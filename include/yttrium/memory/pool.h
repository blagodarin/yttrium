/// \file
/// \brief

#ifndef _include_yttrium_memory_pool_h_
#define _include_yttrium_memory_pool_h_

#include <yttrium/types.h>

#include <cstddef>

namespace Yttrium
{
	///
	class Y_API PoolBase
	{
	public:

		///
		PoolBase(size_t chunk_items, size_t item_size, Allocator* allocator = DefaultAllocator);

		///
		~PoolBase();

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
