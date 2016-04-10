#include "Array.h"
#include <stdlib.h>
#include <assert.h>

#define error(x) exit(1)

struct Array
{
    void** container;
    size_t size_max;
    size_t size_used;
};

Array* Array_create(void)
{
    Array* self = malloc(sizeof(Array));
    if(!self)
        error(ERROR_ALLOC);

    self->container = malloc(sizeof(void*));
    if(!self->container)
		error(ERROR_ALLOC);

    self->size_max = 1;
    self->size_used = 0;
    return self;
}

void Array_destroy(Array** array)
{
	assert(array && (*array));
    free((*array)->container);
    free((*array));
    *array = NULL;
}

size_t Array_size(const Array* self)
{
	assert(self);
    return self->size_used;
}

void* Array_get(const Array* self, size_t index)
{
	assert(self && index < self->size_used);
	return self->container[index];
}

void   Array_remove		(Array* self, size_t index)
{
	assert(self && index < self->size_used 
	&& self->size_used > 0);
	
	for(size_t i = index; i < self->size_used; i++)
		self->container[i] = self->container[i + 1];
	
	void* new_container = realloc(
		self->container,
		sizeof(void*) * (
			(self->size_used > 1) ? 
			(self->size_used - 1) : (1)));
	if(!new_container)
		error(ERROR_ALLOC);
	
	self->container = new_container;
	self->size_max = (
		(self->size_used > 1) ? 
		(self->size_used - 1) : (1));
	self->size_used--;
}

void Array_insert(Array* self, size_t index, void* value)
{
	assert(self && index < self->size_used + 1);
	if(self->size_used >= self->size_max)
	{
		void* new_container = realloc(
			self->container,
			sizeof(void*) * (self->size_max + 1));
		if(!new_container)
			error(ERROR_ALLOC);
		
		self->container = new_container;
		self->size_max++;
	}
	
	for(size_t i = self->size_used; i > index; i--)
		self->container[i] = self->container[i - 1];
	
	self->container[index] = value;
	self->size_used++;
}
