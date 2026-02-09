#ifndef CLIB_VECTOR_H
#define CLIB_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t elem_size;
} clib_vector;

/*--- Lifecycle ---*/
clib_vector *clib_vector_new(size_t elem_size);
void         clib_vector_free(clib_vector *vec);

/*--- Capacity ---*/
size_t clib_vector_size(const clib_vector *vec);
size_t clib_vector_capacity(const clib_vector *vec);
bool   clib_vector_empty(const clib_vector *vec);
int    clib_vector_reserve(clib_vector *vec, size_t capacity);
int    clib_vector_shrink_to_fit(clib_vector *vec);

/*--- Element access ---*/
void *clib_vector_at(const clib_vector *vec, size_t index);
void *clib_vector_front(const clib_vector *vec);
void *clib_vector_back(const clib_vector *vec);
void *clib_vector_data(const clib_vector *vec);

/*--- Modifiers ---*/
int  clib_vector_push_back(clib_vector *vec, const void *elem);
int  clib_vector_pop_back(clib_vector *vec);
int  clib_vector_insert(clib_vector *vec, size_t index, const void *elem);
int  clib_vector_erase(clib_vector *vec, size_t index);
void clib_vector_clear(clib_vector *vec);
int  clib_vector_resize(clib_vector *vec, size_t size);

#endif /* CLIB_VECTOR_H */
