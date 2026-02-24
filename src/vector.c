#include "clib/vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

vector_t *new_vector(size_t capacity) {
	if (capacity == 0) {
		printf("Can't initialize a vector with given capacity");
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

void vector_push_back(vector_t *vector_ptr, void *data) {
	if (vector_ptr == NULL) {
		printf("Vector is NULL");
		return;
	}
	if (vector_ptr->size == vector_ptr->capacity) {
		vector_ptr->capacity = vector_ptr->capacity * 2;
		void *new_allocation = realloc(vector_ptr->data, vector_ptr->capacity * sizeof(void *));
		if (new_allocation == NULL) {
			printf("Can't increase vector's capacity more than %zu", vector_ptr->capacity * sizeof(void *));
			vector_ptr -> capacity = vector_ptr -> capacity / 2;
			return;
		}
		vector_ptr->data = new_allocation;
	}
	vector_ptr->data[vector_ptr->size] = data;
	vector_ptr->size++;
}

void *vector_pop_back(vector_t *vector_ptr) {
	if (vector_ptr == NULL) {
		printf("Vector is NULL");
		return NULL;
	}
	if (vector_ptr->size == 0) {
		return NULL;
	}
	void *element = vector_ptr -> data[--vector_ptr -> size];
	return element;
}

void *vector_at(vector_t *vector_ptr, size_t index);
void *vector_data(vector_t *vector_ptr);