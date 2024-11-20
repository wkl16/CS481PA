#pragma once

#include <stdlib.h>

typedef struct {
    size_t front;
    size_t back;
    size_t max_size;
    int *data;
} Queue;


Queue *new_queue(size_t max_size) {
    Queue *q = malloc(sizeof(Queue));

    q->front = 0;
    q->back = 0;
    q->data = malloc(max_size * sizeof(int));
    q->max_size = max_size;
    
    return q;
}

void free_queue(Queue *q) {
    free(q->data);
    free(q);
}

void enqueue(Queue *q, int item) {
    q->data[q->back] = item;
    q->back = (q->back + 1) % q->max_size;
}

int dequeue(Queue *q) {
    int item = q->data[q->front];
    q->front = (q->front + 1) % q->max_size;
    
    return item;
}
