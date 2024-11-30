#pragma once
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MemorySegment.h"

// Linked List node
struct node {
    long key;         
    struct MemorySegment segment;
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

void initializeHashMap(struct hashMap* mp);
void insert(struct hashMap* mp, long key, struct MemorySegment segment);
void deleteKey(struct hashMap* mp, long key);
int hashFunction(struct hashMap* mp, char* key);
struct MemorySegment* searchHashMap(struct hashMap* mp, long key);
void freeHashMap(struct hashMap* mp);

#endif // HASHMAP_H
