#pragma once

#include "types.h"
#include "memory_types.h"

/// Array of POD objects.
///
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

/// Hash from an uint64_t to POD objects. If you want to use a generic key
/// object, use a hash function to map that object to an uint64_t.
///
/// * Does not call constructors & destructors on elements.
/// * Assumes they can be moved with memmove().
template<typename T> struct Hash
{
public:
	Hash(Allocator &a);
	
	struct Entry {
		uint64_t key;
		uint32_t next;
		T value;
	};

	Array<uint32_t> _hash;
	Array<Entry> _data;
};