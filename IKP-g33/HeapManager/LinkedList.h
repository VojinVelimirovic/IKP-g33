#pragma once
#include "MemorySegment.h"
typedef struct ListNode {
    int address;
    int free_segments;
    struct ListNode* next;
} ListNode;

// Linked list structure
typedef struct LinkedList {
    ListNode* head;
} LinkedList;

// Function declarations
void initList(LinkedList* list);
void append(LinkedList* list, int address, int free_segments);
void printList(const LinkedList* list);
void freeList(LinkedList* list);
void updateList(LinkedList* list, int start_address, int requiredSegments);
void formListFromSegments(LinkedList* list, TMemorySegment* segments, int totalSegments);