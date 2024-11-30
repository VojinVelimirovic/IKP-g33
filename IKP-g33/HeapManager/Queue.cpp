#include <stdio.h>
#include "queue.h"

// Function to initialize the queue
void initializeQueue(Queue* q)
{
    q->front = -1;
    q->rear = 0;
}

// Function to check if the queue is empty
bool isEmpty(Queue* q) 
{ 
    return (q->front == q->rear - 1);
}

// Function to check if the queue is full
bool isFull(Queue* q) 
{ 
    return (q->rear == MAX_SIZE); 
}

// Function to add an element to the queue (Enqueue operation)
void enqueue(Queue* q, int value)
{
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}

// Element se "brise" tako sto se indeks glave poveca za jedno mjesto
void dequeue(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    q->front++;
}

// Function to get the element at the front of the queue (Peek operation)
int peek(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1;
    }
    return q->items[q->front + 1];
}

void printQueue(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }

    printf("Current Queue: ");
    for (int i = q->front + 1; i < q->rear; i++) {
        printf("%d ", q->items[i]);
    }
    printf("\n");
}
