#pragma once
#include "HashMap.h"


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
