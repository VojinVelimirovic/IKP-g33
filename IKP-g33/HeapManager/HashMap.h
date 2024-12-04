#pragma once
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MemorySegment.h"

// value koji se cuva u hashmapi
struct node {
    int key;
    int size;                      // ovo  
    int segmentsTaken;             // treba
    struct MemorySegment segment;  // opet
    struct node* next; // Pointer to the next node
};

// Hash Map structure
struct hashMap {
    int numOfElements; // Current number of elements in the hash map
    int capacity;      // Capacity of the hash map
    struct node** arr; // Array of pointers to linked lists (buckets)
};

// Function declarations

void setNode(struct node* node, char* key, char* value);

void initializeHashMap(struct hashMap* map, int capacity);
void  insertIntoHashMap(struct hashMap* map, int key, struct MemorySegment segment, int size, int segmentsTaken);
void removeFromHashMap(struct hashMap* map, int key);
struct node* searchHashMap(struct hashMap* map, int key);
void freeHashMap(struct hashMap* mp);

int hashFunction(struct hashMap* mp, int key);
#endif // HASHMAP_H
