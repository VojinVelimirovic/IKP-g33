#pragma once

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#define MAX_SIZE 100

// Defining the Queue structure
typedef struct {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Function prototypes

void initializeQueue(Queue* q);

bool isEmpty(Queue* q);

bool isFull(Queue* q);

// Dodavanje element-a na kraj queue
void enqueue(Queue* q, int value);

// Brisanje elementa sa glave
void dequeue(Queue* q);

// Dobijanje elementa sa glave
int peek(Queue* q);

void printQueue(Queue* q);

#endif // QUEUE_H

