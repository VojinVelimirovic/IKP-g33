#include <stdio.h>
#include <stdlib.h>
#include "MemorySegment.h"
#include "Memory.h"

// Global variables
TMemorySegment* segments = NULL;
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

void* allocate_memory(int size) {
    int address = firstFit(size);
    if (address == (int)NULL) {
        return NULL; // Memory allocation failed
    }

    //TODO: malloc(address, size)

    return (void*)address; // Cast the address to a pointer
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
}

// Cleanup memory segments
void cleanup_segments() {
    for (int i = 0; i < segmentCount; i++) {
        CloseHandle(segments[i].mutex); // Close each mutex handle
    }
    free(segments); // Free the memory allocated for segments
}