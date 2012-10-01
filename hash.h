#pragma once

#include "array.h"
#include "collection_types.h"

/// The hash function stores its data in a "list-in-an-array" where
/// indices are used instead of pointers. A free list is used to keep
/// track of free data entries.

namespace hash
{
	template<typename T> void set(Hash<T> &h, uint64_t key, const T &value);
}

namespace hash_internal
{
	const uint32_t END_OF_LIST = 0xffffffffu;
	static const uint64_t UNUSED_KEY = 0xffffffffeeeeeeeeull;

	template<typename T> uint32_t find_or_fail(const Hash<T> &h, uint64_t key)
	{
		if (array::size(h._hash) == 0)
			return END_OF_LIST;

		uint32_t ei = h._hash[key % array::size(h._hash)];
		while (ei != END_OF_LIST) {
			if (h._data[ei].key == key)
				return ei;
			ei = h._data[ei].next;
		}
		return ei;
	}

	template<typename T> uint32_t add_entry(Hash<T> &h, uint64_t key)
	{
		typename Hash<T>::Entry e;
		e.key = key;
		e.next = END_OF_LIST;
		if (h._freelist == END_OF_LIST) {
			uint32_t ei = array::size(h._data);
			array::push_back(h._data, e);
			return ei;
		}
		uint32_t ei = h._freelist;
		h._freelist = h._data[h._freelist].next;
		h._data[ei] = e;
		return ei;
	}

	template<typename T> uint32_t find_or_make(Hash<T> &h, uint64_t key)
	{
		uint32_t i = key % array::size(h._hash);
		if (h._hash[i] == END_OF_LIST) {
			uint32_t ei = add_entry(h, key);
			h._hash[i] = ei;
			return ei;
		}
		uint32_t ei = h._hash[i];
		while (true) {
			if (h._data[ei].key == key)
				return ei;
			uint32_t next = h._data[ei].next;
			if (next == END_OF_LIST) {
				uint32_t next_ei = add_entry(h, key);
				h._data[ei].next = next_ei;
				return next_ei;
			}
			ei = next;
		}
	}

	template<typename T> void erase(Hash<T> &h, uint32_t i, uint32_t prev, uint32_t ei)
	{
		if (prev == END_OF_LIST)
			h._hash[i] = h._data[ei].next;
		else
			h._data[prev].next = h._data[ei].next;
		h._data[ei].key = UNUSED_KEY;
		h._data[ei].next = h._freelist;
		h._freelist = ei;
	}

	template<typename T> void find_and_erase(Hash<T> &h, uint64_t key)
	{
		if (array::size(h._hash) == 0)
			return;

		uint32_t prev = END_OF_LIST;
		uint32_t i = key % array::size(h._hash);
		uint32_t ei = h._hash[i];
		while (ei != END_OF_LIST) {
			if (h._data[ei].key == key) {
				erase(h, i, prev, ei);
				return;
			}
			ei = h._data[ei].next;
		}
	}

	template<typename T> void rehash(Hash<T> &h, uint32_t new_size)
	{
		Hash<T> nh(*h._hash._allocator);
		array::resize(nh._hash, new_size);
		array::reserve(nh._data, array::size(h._data));
		for (uint32_t i=0; i<new_size; ++i)
			nh._hash[i] = END_OF_LIST;
		for (uint32_t i=0; i<array::size(h._data); ++i) {
			const typename Hash<T>::Entry &e = h._data[i];
			if (h._data[i].key == UNUSED_KEY)
				continue;
			hash::set(nh, e.key, e.value);
		}

		Hash<T> empty(*h._hash._allocator);
		h.~Hash<T>();
		memcpy(&h, &nh, sizeof(Hash<T>));
		memcpy(&nh, &empty, sizeof(Hash<T>));
	}

	template<typename T> bool full(const Hash<T> &h)
	{
		const float max_load_factor = 0.7f;
		return array::size(h._data) >= array::size(h._hash) * max_load_factor;
	}

	template<typename T> void grow(Hash<T> &h)
	{
		uint32_t new_size = array::size(h._data) * 2 + 10;
		rehash(h, new_size);
	}
}

namespace hash
{
	const uint32_t END_OF_LIST = 0xffffffffu;

	template<typename T> bool has(const Hash<T> &h, uint64_t key)
	{
		return hash_internal::find_or_fail(h, key) != END_OF_LIST;
	}

	template<typename T> const T &get(const Hash<T> &h, uint64_t key, const T &deffault)
	{
		uint32_t i = hash_internal::find_or_fail(h, key);
		return i == END_OF_LIST ? deffault : h._data[i].value;
	}

	template<typename T> void set(Hash<T> &h, uint64_t key, const T &value)
	{
		if (array::size(h._hash) == 0)
			hash_internal::grow(h);

		uint32_t i = hash_internal::find_or_make(h, key);
		h._data[i].value = value;
		if (hash_internal::full(h))
			hash_internal::grow(h);
	}

	template<typename T> void remove(Hash<T> &h, uint64_t key)
	{
		hash_internal::find_and_erase(h, key);
	}

	template<typename T> void reserve(Hash<T> &h, uint32_t size)
	{
		hash_internal::rehash(h, size);
	}
}

template <typename T> Hash<T>::Hash(Allocator &a) :
	_freelist(END_OF_LIST), _hash(a), _data(a)
{}
