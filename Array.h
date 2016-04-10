#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdbool.h>

#define foreach(iterator, array)                  \
    for(size_t iterator = Array_size(array) - 1;  \
        iterator < Array_size(array); iterator--)

typedef struct Array Array;

Array* Array_create		(void);
void   Array_destroy	(Array** array);

size_t Array_size		(const Array* self);
void*  Array_get		(const Array* self, size_t index);

void   Array_remove		(Array* self, size_t index);
void   Array_insert		(Array* self, size_t index, 
						 void* value);

#endif
