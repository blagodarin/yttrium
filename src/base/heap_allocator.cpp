#include <cstdlib> // free, malloc, realloc
#include <new>     // bad_alloc

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/types.hpp>

// NOTE: We should be GCC-compatible here.

// NOTE: The current implementation lamely uses CRT memory allocation routines.

namespace Yttrium
{

class HeapAllocatorImpl
	: public HeapAllocator
{
public:

	HeapAllocatorImpl() throw()
	{
	}

public: // Allocator

	virtual void *allocate(size_t size, size_t align = 0)
	{
		void *pointer = malloc(size);
		if (!pointer)
		{
			throw std::bad_alloc();
		}

		//__sync_add_and_fetch(&_status.total_bytes, total_bytes);
		//__sync_add_and_fetch(&_status.allocated_bytes, allocated_bytes);
		__sync_add_and_fetch(&_status.allocated_blocks, 1);

		__sync_add_and_fetch(&_status.allocations, 1);

		return pointer;
	}

	virtual void deallocate(void *pointer) throw()
	{
		if (pointer)
		{
			free(pointer);

			__sync_sub_and_fetch(&_status.allocated_blocks, 1);
			//__sync_sub_and_fetch(&_status.allocated_bytes, allocated_bytes);
			//__sync_sub_and_fetch(&_status.total_bytes, total_bytes);

			__sync_add_and_fetch(&_status.deallocations, 1);
		}
	}

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove)
	{
		if (movability == MayNotMove)
		{
			return NULL;
		}

		void *new_pointer = realloc(pointer, size);
		if (!new_pointer)
		{
			throw std::bad_alloc();
		}

//		if (new_total_bytes > total_bytes)
//		{
//			size_t difference = new_total_bytes - total_bytes;
//
//			__sync_add_and_fetch(&_status.total_bytes, difference);
//			__sync_add_and_fetch(&_status.allocated_bytes, difference);
//		}
//		else
//		{
//			size_t difference = total_bytes - new_total_bytes;
//
//			__sync_sub_and_fetch(&_status.allocated_bytes, difference);
//			__sync_sub_and_fetch(&_status.total_bytes, difference);
//		}

		__sync_add_and_fetch(&_status.reallocations, 1);

		return new_pointer;
	}

	virtual Status status() const throw()
	{
		return _status;
	}

private:

	Status _status;
};

static bool _heap_allocator_initialized = false;

static char _heap_allocator_buffer[sizeof(HeapAllocatorImpl)];

HeapAllocator *HeapAllocator::instance()
{
	if (__sync_bool_compare_and_swap(&_heap_allocator_initialized, false, true))
	{
		new(_heap_allocator_buffer) HeapAllocatorImpl();
	}
	return reinterpret_cast<HeapAllocatorImpl *>(_heap_allocator_buffer);
}

} // namespace Yttrium
