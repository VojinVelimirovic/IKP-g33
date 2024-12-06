#include <stdlib.h>
#include <stdbool.h>
#include "HashMap.h"
#include "Memory.h"

// Global memory and hashmaps
TMemorySegment* segments = NULL;
int totalSegments = 0; //trenutan broj segmenata. ovaj broj se menja i nalazi se u ovom fajlu zato sto nam ne treba van njega
HashMap* blockHashMap = NULL;
HashMap* blockAddressHashMap = NULL;

//struktura za povratnu vrednost FirstFit-a
typedef struct FirstFitResult {
    int startIndex;       // Indeks prvog slobodnog segmenta. Ako nema mesta za blok nakon firstFita on ce biti -1
    int missingSegments;  // U koliko nema mesta za blok nakon firstFita ovo nam govori koliko segmenata treba dodati na kraju.
} FirstFitResult;

// Helper: Initialize memory segments
void initializeMemory(int initialSize) {
    segments = (TMemorySegment*)malloc(initialSize * sizeof(TMemorySegment));
    for (int i = 0; i < initialSize; i++) {
        segments[i].address = i;
        segments[i].isFree = true; //na pocetku su segmenti slobodni
        segments[i].mutex = CreateMutex(NULL, FALSE, NULL);
    }
    totalSegments = initialSize;
    blockHashMap = createHashMap(128); //u hashmapi ima ovoliko, ne mora da bude dinamicna mozemo menjati velicinu
    blockAddressHashMap = createHashMap(128);
}

// Helper: Resize memory
void addSegments(int additionalSegments) {
    //ponovo allocuje segmente tako da ih ima vise bez unistavanja postojecih segmenata
    segments = (TMemorySegment*)realloc(segments, (totalSegments + additionalSegments) * sizeof(TMemorySegment));
    for (int i = totalSegments; i < totalSegments + additionalSegments; i++) {
        segments[i].address = i;
        segments[i].isFree = true; //novo dodate segmente postavljam da su slobodni
        segments[i].mutex = CreateMutex(NULL, FALSE, NULL);
    }
    totalSegments += additionalSegments; //menjamo ukupan broj segmenata
}

FirstFitResult firstFit(int size) {
    FirstFitResult result;
    result.startIndex = -1;
    result.missingSegments = 0;
    int requiredSegments = (size + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
    int count = 0;

    for (int i = 0; i < totalSegments; i++) {
        if (segments[i].isFree) {
            count++;
            if (count == requiredSegments) {
                result.startIndex = i - requiredSegments + 1; //Ako je nadjen prostor za blok postavlja se startIndex
                return result;                                //i returnuje se rezultat
            }
        }
        else {
            count = 0;
        }
    }

    // Ako nije prosao return znaci da je count nakon for loopa jednak broju slobodnih segmenata na kraju
    // i takodje znaci da je startIndex ostao -1.
    //allocate_memory ce proveravati da li je startIndex -1. Ako jeste znaci da treba da prosiri broj segmenata
    result.missingSegments = requiredSegments - count;
    return result;
}

void* allocate_memory(int size) {
    FirstFitResult fit = firstFit(size);
    int requiredSegments = (size + SEGMENT_SIZE - 1) / SEGMENT_SIZE;

    if (fit.startIndex == -1) {
        // Ako nije bilo mesta za blok, segmenti se prosiruju za broj segmenata koji fali
        addSegments(fit.missingSegments);
        fit.startIndex = totalSegments - fit.missingSegments; //sada kada je prosiren broj segmenata blok moze da se ugura
    }

    // Jedan po jedan od start indexa pa nadalje se zakljucavaju segmenti, menja im se isFree pa se odkljucavaju
    for (int i = fit.startIndex; i < fit.startIndex + requiredSegments; i++) {
        WaitForSingleObject(segments[i].mutex, INFINITE);
        segments[i].isFree = false;
        ReleaseMutex(segments[i].mutex);
    }

    // Sada stvaramo blok
    TBlock* block = (TBlock*)malloc(sizeof(TBlock));
    block->start_address = fit.startIndex;
    block->size = size;
    block->segments_taken = requiredSegments;

    // Taj blok guramo u blockHashMap. kljuc je start adresa a vrednost je sam blok.
    put(blockHashMap, block->start_address, block);
    // Taj blok takodje guramo u blockAddressHashMap. kljuc i vrednost su start adresa bloka. vrednost cemo kasnije menjati 
    put(blockAddressHashMap, block->start_address, (void*)(intptr_t)block->start_address);

    return (void*)(intptr_t)block->start_address; //return ove funkcije je int konvertovan u (void*) start_addresse
    //ovu return adresu cemo ispisati klijentu na konzolu i on ce nju kasnije da koristi da bi oslobidio blok
}

void free_memory(void* address) {
    // iz blockAddressHashMap izvlacimo trenutnu adresu i konvertujemo je u int
    int current_address = (int)(intptr_t)get(blockAddressHashMap, (intptr_t)address);

    // na osnovu te adrese izblacimo blok iz blockHashMap
    TBlock* block = (TBlock*)get(blockHashMap, current_address);



    // oslobadjamo svaki segment koji je pripadao tom bloku
    for (int i = block->start_address; i < block->start_address + block->segments_taken; i++) {
        // Lock the segment
        WaitForSingleObject(segments[i].mutex, INFINITE);

        // Mark the segment as free
        segments[i].isFree = true;

        // Unlock the segment
        ReleaseMutex(segments[i].mutex);
    }

    // brojimo slobodne segmente
    int freeSegmentsCount = 0;
    for (int i = 0; i < totalSegments; i++) {
        if (segments[i].isFree) {
            freeSegmentsCount++;
        }
    }
    // ako ih je preko 5 brisemo one koji visak slobodnih segmenata sa pocetka
    if (freeSegmentsCount > 5) {
        //broj segmenata koje treba da izbrisemo
        int segmentsToRemove = freeSegmentsCount - 5;

        // prolazimo kroz sve segmente. kad god naidjemo na slobodan segment svim segmentima nakon njega se smanjuje adresa za 1
        // ovo se desava dok ne izbacimo segmentsToRemove broj segmenata
        for (int i = 0; i < totalSegments && segmentsToRemove > 0; i++) {
            if (segments[i].isFree) {
                // Za svaki ZAUZET segment nakon slobodnog segmenta kojeg izbacujemo
                // proveravamo da li postoji blok cija se start adresa podudara sa njegovom adresom
                // ukoliko postoji on ce se izvuci u affectedBlock i tu cemo mu smanjiti start addresu za 1
                for (int j = 0; j < totalSegments; j++) {
                    if (segments[j].isFree == false && segments[j].address > i) {
                        //izbukli smo blok cija se start adresa treba smanjiti za 1
                        TBlock* affectedBlock = (TBlock*)get(blockHashMap, segments[j].address);
                        if (affectedBlock != NULL) {
                            // nasli smo njegovu originalnu adresu
                            // zato sto je njegova trenutna startna adresa vrednost blockAddressHashMape
                            // a kljuc odatle je njegova originalna adresa
                            int original_address = findKeyByValue(blockAddressHashMap, (intptr_t)affectedBlock->start_address);

                            if (original_address != -1) {
                                // Smanjujemo mu trenutnu startnu adresu u blockHashMap
                                affectedBlock->start_address--;

                                // Azuriramo mu trenutnu startnu adresu u blockAddressHashMap na kljucu njegove originalne startne adrese
                                put(blockAddressHashMap, original_address, (void*)(intptr_t)affectedBlock->start_address);
                            }
                        }
                    }
                }


                // unutar segments niza svaki segment se pomera za 1 u nazad i njihova adresa se smanjuje za 1
                for (int j = i; j < totalSegments - 1; j++) {
                    segments[j] = segments[j + 1];
                    segments[j].address--;
                }

                // Ukupan broj segmenata se menja
                totalSegments--;
                segmentsToRemove--;
                i--; //posto su se u segments svi pomerili za 1 u nazad moramo i da pomerimo u nazad
            }
        }
    }
    // Sada bukvalno izbacujemo blok iz obe hash mape

    // za blockHashMapu koristimo njegovu trenutnu adresu koju smo izracunali na pocetku metode
    remove(blockHashMap, current_address); 

    // za blockAddressHashMap koristimo njegovu originalnu adresu koja je zadata kao argument metode
    remove(blockAddressHashMap, (intptr_t)address);                                         
}