#include "clib/queue.h"
#include "clib/vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


queue_t *new_queue(size_t capacity) {
    queue_t *new_queue = malloc(sizeof(queue_t));
    if (new_queue == NULL) {
        fprintf(stderr, "Cant allocate memory for queue!");
        return NULL;
    }
    new_queue -> size = 0;
    new_queue -> capacity = capacity;
    new_queue -> data = malloc(capacity * sizeof(void *));
    if (new_queue -> data == NULL) {
        fprintf(stderr, "Cant allocate memory for queue!");
        free(new_queue);
        return NULL;
    }
    return new_queue;
}

void free_queue(queue_t *queue_ptr) {
    if (queue_ptr == NULL) {
        return;
    }
    free(queue_ptr -> data);
    free(queue_ptr);
}

int queue_is_empty(queue_t *queue_ptr) {
    if (queue_ptr == NULL) {
        fprintf(stderr, "Queue is None");
        return 0;
    }
    if (queue_ptr -> size == 0) {
        return 1;
    }
    return 0;
}

static int increase_queue_capacity(queue_t *queue_ptr, size_t new_capacity) {
    if (queue_ptr == NULL) {
        fprintf(stderr, "Queue is None");
        return 0;
    }
    void *new_data = realloc(queue_ptr -> data, sizeof(void *) * new_capacity);
    if (new_data == NULL) {
        fprintf(stderr, "Can't allocate capacity of %zu bytes", new_capacity);
        return 0;
    }
    queue_ptr -> capacity = new_capacity;
    queue_ptr -> data = new_data;
    return 1;
}

void queue_push(queue_t *queue_ptr, void *data) {
    if (queue_ptr == NULL) {
        fprintf(stderr, "Queue is None");
        return;
    }
    if (queue_ptr -> size == queue_ptr -> capacity) {
        int signal = increase_queue_capacity(queue_ptr, queue_ptr -> capacity * 2);
        if (signal == 0) {
            fprintf(stderr, "Failed to push element!");
            return;
        }
    }   
    queue_ptr -> data [queue_ptr -> size ++] = data;    
}

void *queue_pop(queue_t *queue_ptr) {
    if (queue_is_empty(queue_ptr) == 1) {
        fprintf(stderr, "Queue is Empty");
        return NULL;
    }
    void *element = queue_front(queue_ptr);
    queue_ptr -> size--;
    memmove(&queue_ptr -> data[0], &queue_ptr -> data[1], sizeof(void *) * queue_ptr -> size);
    return element;
}

void *queue_front(queue_t *queue_ptr) {
    if (queue_is_empty(queue_ptr) == 1) {
        fprintf(stderr, "Queue is Empty");
        return NULL;
    }
    return queue_ptr -> data[0];
}

void *queue_back(queue_t *queue_ptr) {
    if (queue_is_empty(queue_ptr) == 1) {
        fprintf(stderr, "Queue is Empty");
        return NULL;
    }
    return queue_ptr -> data [queue_ptr -> size - 1];
}
