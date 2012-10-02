#pragma once

#include "array.h"
#include "collection_types.h"

namespace foundation {

	/// The hash function stores its data in a "list-in-an-array" where
	/// indices are used instead of pointers. 
	///
	/// When items are removed, the array-list is repacked to always keep
	/// it tightly ordered.

	namespace hash
	{
		template<typename T> void set(Hash<T> &h, uint64_t key, const T &value);
	}

	namespace hash_internal
	{
		const uint32_t END_OF_LIST = 0xffffffffu;
		
		struct FindResult
		{
			uint32_t hash_i;
			uint32_t data_prev;
			uint32_t data_i;
		};	

		template<typename T> uint32_t add_entry(Hash<T> &h, uint64_t key)
		{
			typename Hash<T>::Entry e;
			e.key = key;
			e.next = END_OF_LIST;
			uint32_t ei = array::size(h._data);
			array::push_back(h._data, e);
			return ei;
		}

		template<typename T> void erase(Hash<T> &h, const FindResult &fr)
		{
			if (fr.data_prev == END_OF_LIST)
				h._hash[fr.hash_i] = h._data[fr.data_i].next;
			else
				h._data[fr.data_prev].next = h._data[fr.data_i].next;

			if (fr.data_i == array::size(h._data) - 1) {
				array::pop_back(h._data);
				return;
			}

			h._data[fr.data_i] = h._data[array::size(h._data) - 1];
			FindResult last = find(h, h._data[fr.data_i].key);

			if (last.data_prev != END_OF_LIST)
				h._data[last.data_prev].next = fr.data_i;
			else
				h._hash[last.hash_i] = fr.data_i;
		}

		template<typename T> FindResult find(const Hash<T> &h, uint64_t key)
		{
			FindResult fr;
			fr.hash_i = END_OF_LIST;
			fr.data_prev = END_OF_LIST;
			fr.data_i = END_OF_LIST;

			if (array::size(h._hash) == 0)
				return fr;

			fr.hash_i = key % array::size(h._hash);
			fr.data_i = h._hash[fr.hash_i];
			while (fr.data_i != END_OF_LIST) {
				if (h._data[fr.data_i].key == key)
					return fr;
				fr.data_prev = fr.data_i;
				fr.data_i = h._data[fr.data_i].next;
			}
			return fr;
		}

		template<typename T> uint32_t find_or_fail(const Hash<T> &h, uint64_t key)
		{
			return find(h, key).data_i;
		}

		template<typename T> uint32_t find_or_make(Hash<T> &h, uint64_t key)
		{
			FindResult fr = find(h, key);
			if (fr.data_i != END_OF_LIST)
				return fr.data_i;

			fr.data_i = add_entry(h, key);
			if (fr.data_prev == END_OF_LIST)
				h._hash[fr.hash_i] = fr.data_i;
			else
				h._data[fr.data_prev].next = fr.data_i;
			return fr.data_i;
		}	

		template<typename T> void find_and_erase(Hash<T> &h, uint64_t key)
		{
			const FindResult fr = find(h, key);
			if (fr.data_i != END_OF_LIST)
				erase(h, fr);
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

		template<typename T> const typename Hash<T>::Entry *begin(const Hash<T> &h)
		{
			return array::begin(h._data);
		}

		template<typename T> const typename Hash<T>::Entry *end(const Hash<T> &h)
		{
			return array::end(h._data);
		}
	}

	template <typename T> Hash<T>::Hash(Allocator &a) :
		_hash(a), _data(a)
	{}
}