#include <new> // bad_alloc

#include <sys/mman.h> // mmap, mremap, munmap
#include <unistd.h>   // sysconf

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/types.hpp>

// NOTE: We should be GCC-compatible here.

// NOTE: We're using Linux-specific mremap() call here.
// Perhaps we should preprocess it out in non-Linux-compatible environments.

namespace Yttrium
{

class SystemAllocatorImpl
	: public SystemAllocator
{
public:

	SystemAllocatorImpl() throw()
		: _page_size(sysconf(_SC_PAGE_SIZE))
	{
	}

public: // Allocator

	virtual void *allocate(size_t size, size_t align = 0)
	{
		Y_ASSERT(size);

		size_t total_bytes = _page_size * ((reserved_size + size + _page_size - 1) / _page_size);
		size_t allocated_bytes = total_bytes - reserved_size;

		void *base = mmap(NULL, total_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (base == MAP_FAILED)
		{
			throw std::bad_alloc();
		}

		static_cast<size_t *>(base)[0] = total_bytes;
		static_cast<size_t *>(base)[1] = allocated_bytes;

		__sync_add_and_fetch(&_status.total_bytes, total_bytes);
		__sync_add_and_fetch(&_status.allocated_bytes, allocated_bytes);
		__sync_add_and_fetch(&_status.allocated_blocks, 1);

		__sync_add_and_fetch(&_status.allocations, 1);

		return static_cast<char *>(base) + reserved_size;
	}

	virtual void deallocate(void *pointer) throw()
	{
		if (pointer)
		{
			void *base = static_cast<char *>(pointer) - reserved_size;

			size_t total_bytes = static_cast<size_t *>(base)[0];
			size_t allocated_bytes = static_cast<size_t *>(base)[1];

			munmap(base, total_bytes);

			__sync_sub_and_fetch(&_status.allocated_blocks, 1);
			__sync_sub_and_fetch(&_status.allocated_bytes, allocated_bytes);
			__sync_sub_and_fetch(&_status.total_bytes, total_bytes);

			__sync_add_and_fetch(&_status.deallocations, 1);
		}
	}

	virtual void *reallocate(void *pointer, size_t size, Movability movability = MayMove)
	{
		Y_ASSERT(pointer);
		Y_ASSERT(size);

		void *base = static_cast<char *>(pointer) - reserved_size;

		size_t total_bytes = static_cast<size_t *>(base)[0];

		size_t new_total_bytes = _page_size * ((reserved_size + size + _page_size - 1) / _page_size);

		if (new_total_bytes == total_bytes)
		{
			return pointer;
		}

		void *new_base = mremap(base, total_bytes, new_total_bytes, (movability == MayMove ? MREMAP_MAYMOVE : 0));
		if (new_base == MAP_FAILED)
		{
			if (movability == MayMove)
				throw std::bad_alloc();
			return NULL;
		}

		static_cast<size_t *>(new_base)[0] = new_total_bytes;
		static_cast<size_t *>(new_base)[1] = new_total_bytes - reserved_size;

		if (new_total_bytes > total_bytes)
		{
			size_t difference = new_total_bytes - total_bytes;

			__sync_add_and_fetch(&_status.total_bytes, difference);
			__sync_add_and_fetch(&_status.allocated_bytes, difference);
		}
		else
		{
			size_t difference = total_bytes - new_total_bytes;

			__sync_sub_and_fetch(&_status.allocated_bytes, difference);
			__sync_sub_and_fetch(&_status.total_bytes, difference);
		}

		__sync_add_and_fetch(&_status.reallocations, 1);

		return static_cast<char *>(new_base) + reserved_size;
	}

	virtual Status status() const throw()
	{
		return _status;
	}

public: // SystemAllocator

	virtual size_t lower_bound(size_t size) const throw()
	{
		Y_ASSERT(size);

		return _page_size * ((reserved_size + size) / _page_size) - reserved_size;
	}

	virtual size_t upper_bound(size_t size) const throw()
	{
		Y_ASSERT(size);

		return _page_size * ((reserved_size + size + _page_size - 1) / _page_size) - reserved_size;
	}

private:

	static const size_t reserved_size = 32; // This should be sufficient for both 32-bit and 64-bit memory.

private:

	Status _status;
	size_t _page_size;
};

static bool _system_allocator_initialized = false;

static char _system_allocator_buffer[sizeof(SystemAllocatorImpl)];

SystemAllocator *SystemAllocator::instance()
{
	if (__sync_bool_compare_and_swap(&_system_allocator_initialized, false, true))
	{
		new(_system_allocator_buffer) SystemAllocatorImpl();
	}
	return reinterpret_cast<SystemAllocatorImpl *>(_system_allocator_buffer);
}

} // namespace Yttrium
