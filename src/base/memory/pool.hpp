#ifndef __BASE_MEMORY_POOL_HPP
#define __BASE_MEMORY_POOL_HPP

#include <cstddef> // offsetof

#include <Yttrium/pool.hpp>

namespace Yttrium
{

class PoolBase::Private
{
public:

	class Chunk;

	class Chunk
	{
	public:

		class Item
		{
		public:

			Chunk *chunk;
			char   data[0];

		public:

			static Item *base(char *pointer)
			{
				return reinterpret_cast<Item *>(pointer - offsetof(Item, data));
			}

			static size_t size(size_t item_size)
			{
				return offsetof(Item, data) + item_size;
			}
		};

	public:

		Chunk(size_t item_size, size_t chunk_items, Chunk *previous)
			: _previous(previous)
			, _next(nullptr)
			, _size(chunk_items)
			, _end(chunk_items)
			, _free(reinterpret_cast<Item **>(this + 1))
			, _items(reinterpret_cast<char *>(_free + chunk_items))
		{
			char   *raw_item      = _items;
			size_t  raw_item_size = Item::size(item_size);

			for (Item **i = _free; i < reinterpret_cast<Item **>(_items); ++i)
			{
				Item *item = reinterpret_cast<Item *>(raw_item);
				item->chunk = this;
				*i = item;
				raw_item += raw_item_size;
			}

			if (previous)
			{
				previous->_next = this;
			}
		}

		~Chunk()
		{
			if (_previous)
			{
				_previous->_next = _next;
			}

			if (_next)
			{
				_next->_previous = _previous;
			}
		}

	public:

		Item *allocate()
		{
			return _free[--_end];
		}

		void deallocate(Item *item)
		{
			_free[_end++] = item;
		}

		bool is_empty() const
		{
			return (_end == _size);
		}

		bool is_full() const
		{
			return !_end;
		}

	public:

		static size_t size(size_t item_size, size_t chunk_items)
		{
			return sizeof(Chunk)
				+ chunk_items * sizeof(Item **)
				+ chunk_items * Item::size(item_size);
		}

	public:

		Chunk   *_previous;
		Chunk   *_next;
		size_t   _size;
		size_t   _end;
		Item   **_free;
		char    *_items;
	};

public:

	Private(size_t item_size, size_t chunk_items, Allocator *allocator)
		: _allocator(allocator)
		, _item_size(item_size)
		, _chunk_items(chunk_items)
		, _chunk_size(Chunk::size(item_size, chunk_items))
		, _last_chunk(nullptr)
	{
	}

	~Private();

public:

	char *allocate();

	void deallocate(char *pointer);

	char *take();

public:

	Allocator  *_allocator;
	size_t      _item_size;
	size_t      _chunk_items;
	size_t      _chunk_size;
	Chunk      *_last_chunk;
	PoolStatus  _status;
};

} // namespace Yttrium

#endif // __BASE_MEMORY_POOL_HPP
