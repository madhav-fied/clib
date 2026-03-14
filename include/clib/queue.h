#ifndef CLIB_QUEUE_H
#define CLIB_QUEUE_H

#include <stddef.h>

typedef struct Queue {
    size_t size;
    size_t capacity;
    void **data;
} queue_t;

queue_t *new_queue(size_t capacity);
void free_queue(queue_t *queue_ptr);
int queue_is_empty(queue_t *queue_ptr);

void queue_push(queue_t *queue_ptr, void *data);
void *queue_pop(queue_t *queue_ptr);

void *queue_front(queue_t *queue_ptr);
void *queue_back(queue_t *queue_ptr);

#endif