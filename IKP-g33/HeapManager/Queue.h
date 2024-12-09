#pragma once

#ifndef QUEUE_H
#define QUEUE_H

#include <windows.h>
#include <stdbool.h>

typedef struct {
    bool isAllocate;         // True for allocation, false for deallocation
    int value;               // Size to allocate or address to deallocate
    SOCKET clientSocket;
} Request;

typedef struct Node {
    Request request;
    struct Node* next;
} Node;

// Thread-safe queue structure
typedef struct {
    Node* front;
    Node* rear;
    CRITICAL_SECTION lock;
    CONDITION_VARIABLE notEmpty;
} Queue;

// Queue functions
void initializeQueue(Queue* queue);
void enqueue(Queue* queue, Request request);
bool dequeue(Queue* queue, Request* request);
void freeQueue(Queue* queue);

#endif // QUEUE_H


