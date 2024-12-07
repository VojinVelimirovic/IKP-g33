#include "HashMap.h"

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

void initializeMemory(int initialSize);
void addSegments(int additionalSegments);
FirstFitResult firstFit(int size);
void* allocate_memory(int size);
void free_memory(void* address);
void cleanup_segments();
