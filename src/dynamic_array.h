#pragma once

#include <stddef.h>
typedef struct DynamicArray
{
  void *elements;
  size_t num_elements;
  size_t capacity;
  size_t element_size;
} DynamicArray_t;

DynamicArray_t create_array(size_t capacity, size_t element_size);
void free_array(DynamicArray_t *array);

void array_append(DynamicArray_t *array, void *element);

