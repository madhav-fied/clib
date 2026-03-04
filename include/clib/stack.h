#ifndef CLIB_STACK_H
#define CLIB_STACK_H

#include <stddef.h>

typedef struct Stack {
    size_t size;
    size_t capacity;
    void **data;
} stack_t;

stack_t *new_stack(size_t capacity);
void free_stack(stack_t *stack_ptr);
int stack_is_empty(stack_t *stack_ptr);

void stack_push(stack_t *stack_ptr, void *data);
void *stack_pop(stack_t *stack_ptr);

void *stack_top(stack_t *stack_ptr);

#endif