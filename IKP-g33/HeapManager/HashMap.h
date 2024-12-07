#pragma once
#pragma once
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MemorySegment.h"


typedef struct HashMapEntry {
    int key;               // Integer key (address of the block)
    void* value;           // Pointer to the value (Block or int*)
    struct HashMapEntry* next; // For collision resolution (chaining)
} HashMapEntry;

typedef struct HashMap {
    int size;                  // Size of the hash table
    HashMapEntry** table;      // Array of pointers to HashMapEntry
} HashMap;

HashMap* createHashMap(int size);
void put(HashMap* map, int key, void* value);
void* get(HashMap* map, int key);
void deleteHashMap(HashMap* map);
int findKeyByValue(HashMap* map, intptr_t value);
void remove(HashMap* map, int key);
#endif // HASHMAP_H
