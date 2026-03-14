#include "clib/stack.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

stack_t *new_stack(size_t capacity) {
    if (capacity == 0) {
        fprintf(stderr, "Can't allocate stack with 0 capacity!");
        return NULL;
    }
    void *data = malloc(sizeof(void *) * capacity);
    if(data == NULL) {
        fprintf(stderr, "Memory allocation failed!");
        return NULL;
    }
    stack_t * new_stack = malloc(sizeof(stack_t));
    if(new_stack == NULL) {
        fprintf(stderr, "Memory allocation failed!");
        free(data);
        return NULL;
    }
    new_stack -> size = 0;
    new_stack -> capacity = capacity;
    new_stack -> data = data;
    return new_stack;
}

void free_stack(stack_t *stack_ptr) {
    if (stack_ptr == NULL) {
		return;
	}
    free(stack_ptr -> data);
    free(stack_ptr);
}

static int increase_stack_capacity(stack_t *stack_ptr, size_t new_capacity) {
    if (stack_ptr == NULL) {
        fprintf(stderr, "Stack is NULL");
        return 1;
    }
    if (new_capacity <= stack_ptr -> capacity) {
        fprintf(stderr, "Can't decrease the existing capacity");
        return 1;
    }
    void *new_data = realloc(stack_ptr -> data, new_capacity * sizeof(void *));
    if (new_data == NULL) {
        fprintf(stderr, "Failed allocating memory of %zu bytes", new_capacity);
        return 1;
    }
    stack_ptr -> data = new_data;
    stack_ptr -> capacity = new_capacity;
    return 0;
}

int stack_is_empty(stack_t *stack_ptr) {
    if (stack_ptr == NULL) {
        fprintf(stderr, "Stack is NULL");
        return 0;
    }
    if (stack_ptr -> size == 0) {
        return 1;
    }
    return 0;
}

void stack_push(stack_t *stack_ptr, void *data) {
    if (stack_ptr == NULL) {
        fprintf(stderr, "Stack is NULL");
        return;
    }
    if (stack_ptr -> size == stack_ptr -> capacity) {
        int signal = increase_stack_capacity(stack_ptr, stack_ptr -> capacity * 2);
        if (signal == 1) {
            fprintf(stderr, "Failed to push element into stack");
            return;
        }
    }
    stack_ptr -> data[stack_ptr -> size++] = data;
}

void *stack_pop(stack_t *stack_ptr) {
    if (stack_is_empty(stack_ptr) == 1) {
        fprintf(stderr, "Stack is already empty");
        return NULL;
    }
    void *element = stack_ptr -> data[--stack_ptr -> size];
    return element;
}

void *stack_top(stack_t *stack_ptr) {
    if (stack_is_empty(stack_ptr) == 1) {
        fprintf(stderr, "Stack is empty");
        return NULL;
    }
    return stack_ptr -> data[stack_ptr -> size - 1];
}