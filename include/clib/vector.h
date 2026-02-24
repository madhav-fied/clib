#ifndef CLIB_VECTOR_H
#define CLIB_VECTOR_H

#include <stddef.h>

typedef struct Vector {
  size_t size;
  size_t capacity;
  void **data;
} vector_t;

vector_t *new_vector(size_t capacity);
void free_vector(vector_t *vector_ptr);

void vector_push_back(vector_t *vector_ptr, void *data);
void *vector_pop_back(vector_t *vector_ptr);

void *vector_at(vector_t *vector_ptr, size_t index);
void *vector_data(vector_t *vector_ptr);

#endif
