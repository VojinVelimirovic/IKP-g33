#include <stdio.h>
#include <stdlib.h>
#include "MemorySegment.h"
#include "Memory.h"
#include "HashMap.h"

// Global variables
TMemorySegment* segments = NULL;
struct hashMap memoryMap;
int segmentCount = 0;
void* baseMemoryBlock = NULL; // Pointer to the base memory block

int firstFit(int blockSize) {
    // Racunanje broja segmenata potrebnih za blok, -1 je potreban da se ne desi overestimacija
    int requiredSegments = (blockSize + SEGMENT_SIZE - 1) / SEGMENT_SIZE;

    int freeCount = 0;  // brojac kontinualnih slobodnih segmenata
    int startIndex = -1; // index na koji blok treba da se ubaci

    // Prolazimo kroz niz segmenata
    for (int i = 0; i < segmentCount; i++) {
        if (segments[i].isFree) {
            if (freeCount == 0) {
                startIndex = i; // index pocetka novog bloka
            }
            freeCount++;
            if (freeCount == requiredSegments) {
                // Ako nadjemo dovoljno prostora oznacimo segmente kao zauzete
                for (int j = startIndex; j < startIndex + requiredSegments; j++) {
                    segments[j].isFree = false; // Mark segments as occupied
                }

                // Vracamo adresu prvog segmenta u bloku
                return segments[startIndex].address;
            }
        }
        else {
            freeCount = 0; // Reset brojaca ako se naidje na zauzet element, jer lomi kontinualnost
        }
    }

    // Ako nije nadjen blok vracamo null
    return (int)NULL;
}

// funckija za alociranje BLOKA - vraca adresu pocetka bloka 
void* allocate_memory(int size) {
    int address = firstFit(size);
    if (address == (int)NULL) {
        return NULL; // Memory allocation failed
    }

    if (baseMemoryBlock == NULL) {
        return;
    }

    // offset
    void* allocatedMemory = (void*)((char*)baseMemoryBlock + (address - (int)baseMemoryBlock));
    
    // Inicijalizacija memorije na 0 - ne znam da li treba, posto nista ne upisujemo
    memset(allocatedMemory, 0, size);

    // Calculate required segments
    int requiredSegments = (size + SEGMENT_SIZE - 1) / SEGMENT_SIZE;

    // Insert into the hash map
    struct MemorySegment segment = { address, false, NULL };
    insertIntoHashMap(&memoryMap, address, segment, size, requiredSegments);

    return (void*)address;
}

// funckija oslobadja BLOK na datoj adresi
void free_memory(void* address) {
    if (!address) {
        printf("Invalid memory address.\n");
        return;
    }

    // Calculate the starting segment index from the given address
    int startAddress = (int)address;

    // Search for the memory segment in the hash map
    struct node* node = searchHashMap(&memoryMap, startAddress);
    if (!node) {
        printf("Memory address not found in hash map.\n");
        return;
    }

    // Mark associated segments as free
    int startSegmentIndex = (startAddress - (int)baseMemoryBlock) / SEGMENT_SIZE;
    int segmentsToFree = node->segmentsTaken;

    for (int i = startSegmentIndex; i < startSegmentIndex + segmentsToFree; i++) {
        segments[i].isFree = true;
    }

    // Remove the memory record from the hash map
    removeFromHashMap(&memoryMap, startAddress);
}


void initialize_segments(int numSegments) {
    segmentCount = numSegments;
    segments = (TMemorySegment*)malloc(segmentCount * sizeof(TMemorySegment));

    if (!segments) {
        printf("Failed to allocate memory for segment structures.\n");
        exit(EXIT_FAILURE);
    }

    baseMemoryBlock = malloc(segmentCount * SEGMENT_SIZE);
    if (!baseMemoryBlock) {
        printf("Failed to allocate the base memory block.\n");
        free(segments);
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i < segmentCount; i++) {
        segments[i].isFree = true; // Initially, all segments are free
        segments[i].address = (int)((char*)baseMemoryBlock + i * SEGMENT_SIZE); // Compute address dynamically
        segments[i].mutex = CreateMutex(NULL, FALSE, NULL); // Initialize mutex
        if (segments[i].mutex == NULL) {
            printf("Failed to create mutex for segment %d\n", i);
            free(baseMemoryBlock);
            free(segments);
            exit(EXIT_FAILURE);
        }
    }

    initializeHashMap(&memoryMap, 10);
}

// Funckija koja oslobadja sve segmente
void cleanup_segments() {
    for (int i = 0; i < segmentCount; i++) {
        CloseHandle(segments[i].mutex); // Close each mutex handle
    }
    free(segments);
    free(baseMemoryBlock);

    // Oslobadjanje hashmape
    for (int i = 0; i < memoryMap.capacity; i++) {
        struct node* current = memoryMap.arr[i];
        while (current) {
            struct node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(memoryMap.arr);
}