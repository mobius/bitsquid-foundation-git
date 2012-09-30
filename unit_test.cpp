#include "array.h"
#include "memory.h"

#include <stdio.h>
#include <assert.h>

#define ASSERT(x) assert(x)

namespace {
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
}

int main(int, char **)
{
	test_memory();
	test_array();
	test_scratch();
	return 0;
}