#include "hashmap.h"
#define __INT16_MAX__ 32767


// Sets a node with key and value
/*void setNode(struct node* node, char* key, char* value) {
    node->key = key;
    node->value = value;
    node->next = NULL;
}*/

// Initializes the hash map
void initializeHashMap(struct hashMap* hashMap) {
    hashMap->capacity = 100; // Default capacity
    hashMap->numOfElements = 0;

    hashMap->arr = (struct node**)malloc(sizeof(struct node*) * hashMap->capacity);
    for (int i = 0; i < hashMap->capacity; i++) {
        hashMap->arr[i] = NULL; // Initialize buckets to NULL
    }
}

int hashFunction(struct hashMap* mp, long key) {
    return key % mp->capacity; // Return bucket index based on the address
}


// Inserts a key-value pair into the hash map
void insert(struct hashMap* mp, long key, struct MemorySegment segment) {
    int bucketIndex = hashFunction(mp, key);
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    
    newNode->key = key; // Set the key (address)
    newNode->segment = segment; // Set the MemorySegment
    newNode->next = mp->arr[bucketIndex]; // Insert at the beginning
    mp->arr[bucketIndex] = newNode;
}

// Deletes a key from the hash map
void deleteKey(struct hashMap* mp, long key) {
    int bucketIndex = hashFunction(mp, key);
    struct node* prevNode = NULL;
    struct node* currNode = mp->arr[bucketIndex];

    while (currNode != NULL) {
        if (currNode->key == key) {
            if (prevNode == NULL) { // Head node
                mp->arr[bucketIndex] = currNode->next;
            }
            else {
                prevNode->next = currNode->next; // Bypass the node
            }
            free(currNode); // Free the node
            return;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
}

// Searches for a MemorySegment by address in the hash map
struct MemorySegment* searchHashMap(struct hashMap* mp, long key) {
    int bucketIndex = hashFunction(mp, key);
    struct node* bucketHead = mp->arr[bucketIndex];
    while (bucketHead != NULL) {
        if (bucketHead->key == key) {
            return &bucketHead->segment; // Return the MemorySegment
        }
        bucketHead = bucketHead->next;
    }
    return NULL; // Not found
}

// Frees the memory allocated for the hash map
void freeHashMap(struct hashMap* mp) {
    for (int i = 0; i < mp->capacity; i++) {
        struct node* currNode = mp->arr[i];
        while (currNode != NULL) {
            struct node* temp = currNode;
            currNode = currNode->next;
            free(temp); // Free node
        }
    }
    free(mp->arr); // Free the array of pointers
}
