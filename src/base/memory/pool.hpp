#ifndef __BASE_MEMORY_POOL_HPP
#define __BASE_MEMORY_POOL_HPP

#include <cstddef> // offsetof

#include <Yttrium/pool.hpp>

namespace Yttrium
{

class PoolBase::Chunk
{
public:

	class Item
	{
	public:

		Chunk *chunk;
		char   data[0];

	public:

		static Item *base(void *pointer);

		static size_t size(size_t item_size);
	};

public:

	Chunk(size_t chunk_items, size_t item_size, Chunk *previous);

	~Chunk();

public:

	Item *allocate();

	void deallocate(Item *item);

	bool is_empty() const;

	bool is_full() const;

public:

	static size_t size(size_t chunk_items, size_t item_size);

public:

	Chunk   *_previous;
	Chunk   *_next;
	size_t   _size;
	size_t   _end;
	Item   **_free;
	char    *_items;
};

} // namespace Yttrium

#endif // __BASE_MEMORY_POOL_HPP