#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "MemorySegment.h"

// Function to initialize a MemorySegment
void initializeMemorySegment(struct MemorySegment* segment, long address, int size, bool status) {
    segment->address = address;
    segment->size = size;
    segment->status = status;
    segment->mutex = CreateMutex(NULL, FALSE, NULL); // Initialize the mutex
    if (segment->mutex == NULL) {
        fprintf(stderr, "Failed to create mutex\n");
        exit(EXIT_FAILURE);
    }
}

// Function to free a MemorySegment
void freeMemorySegment(struct MemorySegment* segment) {
    CloseHandle(segment->mutex); // Destroy the mutex
}