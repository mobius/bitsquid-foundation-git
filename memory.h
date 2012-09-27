#pragma once

#include "types.h"
#include "memory_types.h"

/// Base class for memory allocators.
///
/// Note: Regardless of which allocator is used, prefer to allocate memory in larger chunks
/// instead of in many small allocations. This helps with data locality, fragmentation,
/// memory usage tracking, etc.
class Allocator
{
public:
	/// Default alignment for memory allocations.
	static const uint32_t DEFAULT_ALIGN = 4;

	Allocator() {}
	virtual ~Allocator() {}
	
	virtual void *allocate(uint32_t size, uint32_t align = DEFAULT_ALIGN) = 0;
	virtual void deallocate(void *p) = 0;
	virtual uint32_t allocated_size(void *p) = 0;
	virtual uint32_t total_allocated() = 0;

private:
    Allocator(const Allocator& other);
    Allocator& operator=(const Allocator& other);
};

#define MAKE_NEW(a, T, ...)		(new ((a).allocate(sizeof(T), alignof(T))) T(__VA_ARGS__))
#define MAKE_DELETE(a, T, p)	do {if (p) {(p)->~T(); a.deallocate(p);}} while (0)

namespace memory_globals {
	void init();
	Allocator &default_allocator();
	void shutdown();
}