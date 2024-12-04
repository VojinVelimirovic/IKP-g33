#include "hashmap.h"
#define __INT16_MAX__ 32767


// Sets a node with key and value
/*void setNode(struct node* node, char* key, char* value) {
    node->key = key;
    node->value = value;
    node->next = NULL;
}*/

// Initializes the hash map
void initializeHashMap(struct hashMap* map, int capacity) {
    map->numOfElements = 0;
    map->capacity = capacity;
    map->arr = (struct node**)malloc(capacity * sizeof(struct node*));
    if (!map->arr) {
        printf("Failed to allocate memory for hash map buckets.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < capacity; i++) {
        map->arr[i] = NULL;
    }
}


// Inserts a key-value pair into the hash map
void insertIntoHashMap(struct hashMap* map, int key, struct MemorySegment segment, int size, int segmentsTaken) {
    int bucketIndex = hashFunction(key, map->capacity);
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    if (!newNode) {
        printf("Failed to allocate memory for new hash map node.\n");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->segment = segment;
    newNode->size = size;
    newNode->segmentsTaken = segmentsTaken;
    newNode->next = map->arr[bucketIndex];
    map->arr[bucketIndex] = newNode;
    map->numOfElements++;
}

struct node* searchHashMap(struct hashMap* map, int key) {
    int bucketIndex = hashFunction(key, map->capacity);
    struct node* current = map->arr[bucketIndex];
    while (current) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Key not found
}


void removeFromHashMap(struct hashMap* map, int key) {
    int bucketIndex = hashFunction(key, map->capacity);
    struct node* current = map->arr[bucketIndex];
    struct node* prev = NULL;

    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            }
            else {
                map->arr[bucketIndex] = current->next;
            }
            free(current);
            map->numOfElements--;
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Key %ld not found in hash map.\n", key);
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


int hashFunction(int key, int capacity) {
    return key % capacity;
}
