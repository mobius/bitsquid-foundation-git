#include "murmur_hash.h"
#include "hash.h"
#include "temp_allocator.h"
#include "array.h"
#include "memory.h"

#include <stdio.h>
#include <assert.h>

#define ASSERT(x) assert(x)

namespace {
	using namespace foundation;
	
	void test_memory() {
		memory_globals::init();
		Allocator &a = memory_globals::default_allocator();

		void *p = a.allocate(100);
		ASSERT(a.allocated_size(p) >= 100);
		ASSERT(a.total_allocated() >= 100);
		void *q = a.allocate(100);
		ASSERT(a.allocated_size(q) >= 100);
		ASSERT(a.total_allocated() >= 200);
		
		a.deallocate(p);
		a.deallocate(q);
		
		memory_globals::shutdown();
	}

	void test_array() {
		memory_globals::init();
		Allocator &a = memory_globals::default_allocator();

		{
			Array<int> v(a);

			ASSERT(array::size(v) == 0);
			array::push_back(v, 3);
			ASSERT(array::size(v) == 1);
			ASSERT(v[0] == 3);

			Array<int> v2(v);
			ASSERT(v2[0] == 3);
			v2[0] = 5;
			ASSERT(v[0] == 3);
			ASSERT(v2[0] == 5);
			v2 = v;
			ASSERT(v2[0] == 3);
			
			ASSERT(array::end(v) - array::begin(v) == array::size(v));
			ASSERT(*array::begin(v) == 3);
			array::pop_back(v);
			ASSERT(array::empty(v));

			for (int i=0; i<100; ++i)
				array::push_back(v, i);
			ASSERT(array::size(v) == 100);
		}

		memory_globals::shutdown();
	}

	void test_scratch() {
		memory_globals::init(256*1024);
		Allocator &a = memory_globals::default_scratch_allocator();

		char *p = (char *)a.allocate(10*1024);

		char *pointers[100];
		for (int i=0; i<100; ++i)
			pointers[i] = (char *)a.allocate(1024);
		for (int i=0; i<100; ++i)
			a.deallocate(pointers[i]);

		a.deallocate(p);

		for (int i=0; i<100; ++i)
			pointers[i] = (char *)a.allocate(4*1024);
		for (int i=0; i<100; ++i)
			a.deallocate(pointers[i]);

		memory_globals::shutdown();
	}

	void test_temp_allocator() {
		memory_globals::init();
		{
			TempAllocator128 ta;
			Array<int> a(ta);
			for (int i=0; i<100; ++i)
				array::push_back(a, i);
			ta.allocate(2*1024);
		}
		memory_globals::shutdown();
	}

	void test_hash() {
		memory_globals::init();
		{
			TempAllocator128 ta;
			Hash<int> h(ta);
			ASSERT(hash::get(h,0,99) == 99);
			ASSERT(!hash::has(h, 0));
			hash::remove(h, 0);
			hash::set(h, 1000, 123);
			ASSERT(hash::get(h,1000,0) == 123);
			ASSERT(hash::get(h,2000,99) == 99);

			for (int i=0; i<100; ++i)
				hash::set(h, i, i*i);
			for (int i=0; i<100; ++i)
				ASSERT(hash::get(h,i,0) == i*i);
			hash::remove(h, 1000);
			ASSERT(!hash::has(h, 1000));
			hash::remove(h, 2000);
			ASSERT(hash::get(h,1000,0) == 0);
			for (int i=0; i<100; ++i)
				ASSERT(hash::get(h,i,0) == i*i);
		}
		memory_globals::shutdown();
	}

	void test_murmur_hash()
	{
		const char *s = "test_string";
		uint64_t h = murmur_hash_64(s, strlen(s), 0);
		ASSERT(h == 0xe604acc23b568f83ull);
	}
}

int main(int, char **)
{
	test_memory();
	test_array();
	test_scratch();
	test_temp_allocator();
	test_hash();
	test_murmur_hash();
	return 0;
}