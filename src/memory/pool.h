#ifndef _src_memory_pool_h_
#define _src_memory_pool_h_

#include <yttrium/pool.h>

#include <cstddef>

namespace Yttrium
{
	class Y_PRIVATE PoolBase::Chunk
	{
	public:

		struct Item
		{
			Chunk* chunk;
			char   data[0];

			static Item* base(void* pointer);
			static size_t size(size_t item_size);
		};

	public:

		Chunk(size_t chunk_items, size_t item_size, Chunk* previous);

		~Chunk();

	public:

		Item* allocate();

		void deallocate(Item* item);

		bool is_empty() const;

		bool is_full() const;

	public:

		static size_t size(size_t chunk_items, size_t item_size);

	public:

		Chunk* _previous;
		Chunk* _next;
		size_t _size;
		size_t _end;
		Item** _free;
		char*  _items;
	};
}

#endif
