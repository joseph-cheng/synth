#include "dynamic_array.h"
#include <stdlib.h>
#include <string.h>

DynamicArray_t create_array(size_t capacity, size_t element_size)
{
  DynamicArray_t array;

  array.elements = calloc(capacity, element_size);
  array.num_elements = 0;
  array.capacity = capacity;
  array.element_size = element_size;

  return array;
}

void free_array(DynamicArray_t *array)
{
  free(array->elements);
}

void array_append(DynamicArray_t *array, void *element)
{
  if (array->num_elements >= array->capacity)
  {
    array->capacity *= 2;
    array->elements = realloc(array->elements, array->capacity * array->element_size);
  }

  memcpy(array->elements + array->num_elements * array->element_size, element, array->element_size);
  array->num_elements++;
}
