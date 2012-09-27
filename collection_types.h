#pragma once

#include "types.h"
#include "memory_types.h"

/// Array of POD objects.
/// * Does not call constructors & destructors on elements.
/// * Assumes they can be moved with memmove().
template<typename T> struct Array
{
	Array(Allocator &a);
	~Array();
	Array(const Array &other);
	Array &operator=(const Array &other);
	
	T &operator[](uint32_t i);
	const T &operator[](uint32_t i) const;

	Allocator *_allocator;
	uint32_t _size;
	uint32_t _capacity;
	T *_data;
};