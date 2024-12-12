#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"

void initList(LinkedList* list) {
    list->head = NULL;
}

// Append a new node to the linked list
void append(LinkedList* list, int address, int free_segments) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (!newNode) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->address = address;
    newNode->free_segments = free_segments;
    newNode->next = NULL;

    if (!list->head) {
        list->head = newNode;
    }
    else {
        ListNode* current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Print the linked list
void printList(const LinkedList* list) {
    ListNode* current = list->head;
    while (current) {
        printf("%d -> ", current->address);
        current = current->next;
    }
    printf("NULL\n");
}

// Free the memory allocated for the linked list
void freeList(LinkedList* list) {
    ListNode* current = list->head;
    ListNode* nextNode;
    while (current) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    free(list);
}

void updateList(LinkedList* list, int start_address, int requiredSegments) {
    if (!list || !list->head) {
        printf("Error: The list is empty or uninitialized.\n");
        return;
    }

    ListNode* current = list->head;
    ListNode* prev = NULL;

    while (current) {
        // Check if the current node matches the block start address
        if (current->address == start_address) {
            current->address += requiredSegments;
            current->free_segments -= requiredSegments;
            return;
        }

        // Check if the block should be inserted in a gap
        if (current->address > start_address) {
            ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
            if (!newNode) {
                perror("Failed to allocate memory for new node");
                exit(EXIT_FAILURE);
            }

            newNode->address = start_address + requiredSegments; // Remaining segments start here
            newNode->free_segments = current->address - newNode->address;
            newNode->next = current;

            if (prev) {
                prev->next = newNode;
            }
            else {
                list->head = newNode;
            }
            return;
        }

        prev = current;
        current = current->next;
    }

    // If the block address is beyond the last node, append it to the list
    append(list, start_address + requiredSegments, requiredSegments);
}

void formListFromSegments(LinkedList* list, TMemorySegment* segments, int totalSegments) {
    if (!list || !segments) {
        printf("Error: Invalid list or segments array.\n");
        return;
    }

    initList(list);

    int i = 0;
    while (i < totalSegments) {
        if (segments[i].isFree) {
            int startAddress = segments[i].address;
            int freeSegmentsCount = 0;

            // Count consecutive free segments
            while (i < totalSegments && segments[i].isFree) {
                freeSegmentsCount++;
                i++;
            }

            append(list, startAddress, freeSegmentsCount);
        }
        else {
            i++;
        }
    }
}