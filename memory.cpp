#include "memory.h"

#include <assert.h>
#include <new>

namespace {
	class MallocAllocator : public Allocator
	{
		struct Header {
			uint32_t size;
		};

		static const uint32_t HEADER_PAD_VALUE = 0xffffffffu;

		uint32_t _total_allocated;

		static inline uint32_t size_with_header(uint32_t size, uint32_t align) {
			return size + align + sizeof(Header);
		}

		static inline void *data_pointer(Header *header, uint32_t align) {
			const void *p = header + 1;
			uintptr_t pi = uintptr_t(p);
			const uint32_t mod = pi % align;
			if (mod)
				pi += (align - mod);
			return (void *)pi;
		}

		static inline void fill(Header *header, void *data, uint32_t size)
		{
			header->size = size;
			uint32_t *p = (uint32_t *)(header + 1);
			while (p < data)
				*p++ = HEADER_PAD_VALUE;
		}

		static inline Header *header(void *data)
		{
			uint32_t *p = (uint32_t *)data;
			while (p[-1] == HEADER_PAD_VALUE)
				--p;
			return (Header *)p - 1;
		}

	public:
		MallocAllocator() : _total_allocated(0) {}

		~MallocAllocator() {
			assert(_total_allocated == 0);
		}

		virtual void *allocate(uint32_t size, uint32_t align) {
			const uint32_t ts = size_with_header(size, align);
			Header *h = (Header *)malloc(ts);
			void *p = data_pointer(h, align);
			fill(h, p, ts);
			_total_allocated += ts;
			return p;
		}

		virtual void deallocate(void *p) {
			if (!p)
				return;

			Header *h = header(p);
			_total_allocated -= h->size;
			free(h);
		}

		virtual uint32_t allocated_size(void *p) {
			return header(p)->size;
		}

		virtual uint32_t total_allocated() {
			return _total_allocated;
		}
	};

	struct MemoryGlobals {
		static const int ALLOCATOR_MEMORY = sizeof(MallocAllocator);
		uint8_t buffer[ALLOCATOR_MEMORY];

		MallocAllocator *default_allocator;

		MemoryGlobals() : default_allocator(0) {}
	};

	MemoryGlobals _memory_globals;
}

namespace memory_globals {
	void init() {
		uint8_t *p = _memory_globals.buffer;
		_memory_globals.default_allocator = new (p) MallocAllocator();
	}

	Allocator &default_allocator() {
		return *_memory_globals.default_allocator;
	}

	void shutdown() {
		_memory_globals.default_allocator->~MallocAllocator();
		_memory_globals = MemoryGlobals();
	}
}


