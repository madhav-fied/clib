#include "clib/vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vector_t *new_vector(size_t capacity) {
	if (capacity == 0) {
		fprintf(stderr, "Can't initialize a vector with given capacity");
		return NULL;
	}
	vector_t *vector_ptr = malloc(sizeof(vector_t));
	if (vector_ptr == NULL) {
		return NULL;
	}
	vector_ptr->size = 0;
	vector_ptr->capacity = capacity;
	vector_ptr->data = malloc(sizeof(void *) * capacity);
	if (vector_ptr->data == NULL) {
		free(vector_ptr);
		return NULL;
	}
	return vector_ptr;
}

void free_vector(vector_t *vector_ptr) {
	if (vector_ptr == NULL) {
		return;
	}
	free(vector_ptr->data);
	free(vector_ptr);
}

int vector_is_empty(vector_t *vector_ptr) {
	if (vector_ptr -> size == 0) {
		return 1;
	}
	return 0;
}

static void vector_increase_capacity(vector_t *vector_ptr, size_t new_capacity) {
	if (vector_ptr -> capacity > new_capacity) {
		return;
	}
	size_t old_capacity = vector_ptr -> capacity;
	vector_ptr -> capacity = new_capacity;
	void *new_allocation = realloc(vector_ptr->data, vector_ptr->capacity * sizeof(void *));
	if (new_allocation == NULL) {
		fprintf(stderr, "Can't increase vector's capacity more than %zu", vector_ptr->capacity * sizeof(void *));
		vector_ptr -> capacity = old_capacity;
		return;
	}
	vector_ptr->data = new_allocation;
}

void vector_push_back(vector_t *vector_ptr, void *data) {
	if (vector_ptr == NULL) {
		fprintf(stderr, "Vector is NULL");
		return;
	}
	if (vector_ptr->size == vector_ptr->capacity) {
		vector_increase_capacity(vector_ptr, vector_ptr -> capacity * 2);
	}
	vector_ptr->data[vector_ptr->size] = data;
	vector_ptr->size++;
}

void *vector_pop_back(vector_t *vector_ptr) {
	if (vector_ptr == NULL) {
		fprintf(stderr, "Vector is NULL");
		return NULL;
	}
	if (vector_ptr->size == 0) {
		return NULL;
	}
	void *element = vector_ptr -> data[--vector_ptr -> size];
	return element;
}

void *vector_at(vector_t *vector_ptr, size_t index) {
	if (index >= vector_ptr -> size) {
		return NULL;
	}
	return vector_ptr -> data[index];
}

void vector_insert(vector_t *vector_ptr, size_t index, void *data) {
	if (index > vector_ptr -> size) {
		fprintf(stderr, "Out of bounds");
		return;
	}

	if (vector_ptr -> size == vector_ptr -> capacity) {
		vector_increase_capacity(vector_ptr, vector_ptr -> capacity * 2);
	}

	size_t bytes_to_shift = sizeof(void *) * (vector_ptr -> size - index);
	memmove(&vector_ptr -> data[index + 1], &vector_ptr -> data[index], bytes_to_shift);
	vector_ptr -> data[index] = data;
	vector_ptr -> size++;	
}

void vector_erase(vector_t *vector_ptr, size_t index) {
	if (index >= vector_ptr -> size) {
		fprintf(stderr, "Out of bounds");
		return;
	}

	size_t bytes_to_shift = sizeof(void *) * ((vector_ptr -> size - index) - 1);
	memmove(&vector_ptr -> data[index], &vector_ptr -> data[index + 1], bytes_to_shift);
	vector_ptr -> size--;
}
