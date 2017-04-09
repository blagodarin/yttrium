/// \file
/// \brief

#ifndef _include_yttrium_memory_pool_h_
#define _include_yttrium_memory_pool_h_

#include <yttrium/api.h>

#include <cstddef>

namespace Yttrium
{
	///
	class Y_API PoolBase
	{
	public:
		///
		PoolBase(size_t chunk_items, size_t item_size) noexcept;

		///
		~PoolBase();

		PoolBase(const PoolBase&) = delete;
		PoolBase& operator=(const PoolBase&) = delete;

	protected:
		void* allocate();
		void deallocate(void*) noexcept;
		void* take() noexcept;

	private:
		const size_t _chunk_items;
		const size_t _item_size;
		const size_t _chunk_size;
		class PoolChunk* _last_chunk = nullptr;
	};

	///
	template <typename T>
	class Pool : public PoolBase
	{
	public:
		///
		explicit Pool(size_t chunk_items = 32) noexcept
			: PoolBase{chunk_items, sizeof(T)}
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
		void clear() noexcept
		{
			for (auto pointer = take(); pointer; pointer = take())
			{
				static_cast<T*>(pointer)->~T();
				PoolBase::deallocate(pointer);
			}
		}

		///
		void deallocate(T* pointer) noexcept
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
