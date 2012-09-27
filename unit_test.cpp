#include "memory.h"

#include <stdio.h>
#include <assert.h>

#define CHECK(x) assert(x)

namespace {
	void test_memory() {
		memory_globals::init();
		Allocator &a = memory_globals::default_allocator();

		void *p = a.allocate(100);
		CHECK(a.allocated_size(p) >= 100);
		CHECK(a.total_allocated() >= 100);
		void *q = a.allocate(100);
		CHECK(a.allocated_size(q) >= 100);
		CHECK(a.total_allocated() >= 200);

		a.deallocate(p);
		a.deallocate(q);

		CHECK(a.total_allocated() == 0);

		memory_globals::shutdown();
	}
}

int main(int, char **)
{
	test_memory();
}