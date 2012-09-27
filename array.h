#pragma once

#include "collection_types.h"
#include "memory.h"

#include <memory>

// TODO:
// * insert
// * erase

namespace array
{
	template<typename T> inline uint32_t size(const Array<T> &a) 		{return a._size;}
	template<typename T> inline bool any(const Array<T> &a) 			{return a._size != 0;}
	template<typename T> inline bool empty(const Array<T> &a) 			{return a._size == 0;}
	template<typename T> inline uint32_t capacity(const Array<T> &a)	{return a._capacity;}

	template<typename T> inline T* begin(Array<T> &a) 					{return a._data;}
	template<typename T> inline const T* begin(const Array<T> &a) 		{return a._data;}
	template<typename T> inline T* end(Array<T> &a) 					{return a._data + a._size;}
	template<typename T> inline const T* end(const Array<T> &a) 		{return a._data + a._size;}
	
	template<typename T> inline T& front(Array<T> &a) 					{return a._data[0];}
	template<typename T> inline const T& front(const Array<T> &a) 		{return a._data[0];}
	template<typename T> inline T& back(Array<T> &a) 					{return a._data[a._size-1];}
	template<typename T> inline const T& back(const Array<T> &a) 		{return a._data[a._size-1];}

	template <typename T> inline void clear(Array<T> &a) {resize(a,0);}
	template <typename T> inline void trim(Array<T> &a) {set_capacity(a,a._size);}

	template <typename T> void resize(Array<T> &a, uint32_t new_size)
	{
		if (new_size > a._capacity)
			grow(a, new_size);
		a._size = new_size;
	}

	template <typename T> inline void reserve(Array<T> &a, uint32_t new_capacity)
	{
		if (new_capacity > a._capacity)
			set_capacity(a, new_capacity);
	}

	template<typename T> void set_capacity(Array<T> &a, uint32_t new_capacity)
	{
		if (new_capacity == a._capacity)
			return;

		if (new_capacity < a._size)
			resize(a, new_capacity);

		T *new_data = 0;
		if (new_capacity > 0) {
			new_data = (T *)a._allocator->allocate(sizeof(T)*new_capacity, alignof(T));
			memcpy(new_data, a._data, sizeof(T)*a._size);
		}
		a._allocator->deallocate(a._data);
		a._data = new_data;
		a._capacity = new_capacity;
	}

	template<typename T> void grow(Array<T> &a, uint32_t min_capacity = 0)
	{
		uint32_t new_capacity = a._capacity*2 + 10;
		if (new_capacity < min_capacity)
			new_capacity = min_capacity;
		set_capacity(a, new_capacity);
	}

	template<typename T> inline void push_back(Array<T> &a, const T &item)
	{
		if (a._size + 1 > a._capacity)
			grow(a);
		a._data[a._size++] = item;
	}

	template<typename T> inline void pop_back(Array<T> &a)
	{
		a._size--;
	}
}

template <typename T>
inline Array<T>::Array(Allocator &allocator) : _allocator(&allocator), _size(0), _capacity(0), _data(NULL) {}

template <typename T>
inline Array<T>::~Array()
{
	_allocator->deallocate(_data);
}

template <typename T>
Array<T>::Array(const Array<T> &other) : _allocator(other._allocator), _size(0), _capacity(0), _data(0)
{
	const uint32_t n = other._size;
	array::set_capacity(*this, n);
	memcpy(_data, other._data, sizeof(T) * n);
	_size = n;
}

template <typename T>
Array<T> &Array<T>::operator=(const Array<T> &other)
{
	const uint32_t n = other._size;
	array::resize(*this, n);
	memcpy(_data, other._data, sizeof(T)*n);
	return *this;
}

template <typename T>
inline T & Array<T>::operator[](uint32_t i)
{
	return _data[i];
}

template <typename T>
inline const T & Array<T>::operator[](uint32_t i) const
{
	return _data[i];
}