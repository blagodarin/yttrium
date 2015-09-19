/// \file
/// \brief

#ifndef _include_yttrium_pool_h_
#define _include_yttrium_pool_h_

#include <yttrium/base.h>
#include <yttrium/types.h>

#include <cstddef>

namespace Yttrium
{
	/// %Pool status.
	struct PoolStatus
	{
		size_t allocated_items = 0; ///< Number of allocated items.
		size_t allocated_chunks = 0; ///< Number of allocated chunks.

		size_t item_allocations = 0; ///< Lifetime (wrapping) number of item allocations.
		size_t chunk_allocations = 0; ///< Lifetime (wrapping) number of chunk allocations.

		size_t item_deallocations = 0; ///< Lifetime (wrapping) number of item deallocations.
		size_t chunk_deallocations = 0; ///< Lifetime (wrapping) number of chunk deallocations.
	};

	///
	class Y_API PoolBase
	{
	public:

		///
		PoolBase(size_t chunk_items, size_t item_size, Allocator* allocator = DefaultAllocator);

		///
		~PoolBase();

		///
		PoolStatus status() const;

		PoolBase(const PoolBase&) = delete;
		PoolBase& operator=(const PoolBase&) = delete;

	protected:

		///
		void* allocate();

		///
		void deallocate(void* pointer);

		///
		void* take();

	private:

		class Chunk;

		Allocator* _allocator;
		size_t     _chunk_items;
		size_t     _item_size;
		size_t     _chunk_size;
		Chunk*     _last_chunk;
		PoolStatus _status;
	};

	///
	template <typename T>
	class Pool : public PoolBase
	{
	public:

		///
		Pool(size_t chunk_items = 32, Allocator* allocator = DefaultAllocator)
			: PoolBase(chunk_items, sizeof(T), allocator)
		{
		}

		///
		~Pool()
		{
			clear();
		}

		///
		T* allocate()
		{
			return static_cast<T*>(PoolBase::allocate());
		}

		///
		void clear()
		{
			for (void* pointer = take(); pointer; pointer = take())
			{
				static_cast<T*>(pointer)->~T();
				PoolBase::deallocate(pointer);
			}
		}

		///
		void deallocate(T* pointer)
		{
			if (pointer)
			{
				pointer->~T();
				PoolBase::deallocate(pointer);
			}
		}
	};
}

#endif
