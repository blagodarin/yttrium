#include "pool.h"

#include <yttrium/memory/allocator.h>

// TODO: Get rid of magical chunk item count in places where pools are used.
// Perhaps setting the pool chunk size to memory page size will do.

namespace Yttrium
{
	PoolBase::Chunk::Item* PoolBase::Chunk::Item::base(void* pointer)
	{
		return reinterpret_cast<Item*>(static_cast<char*>(pointer) - offsetof(Item, data));
	}

	size_t PoolBase::Chunk::Item::size(size_t item_size)
	{
		return offsetof(Item, data) + item_size;
	}

	PoolBase::Chunk::Chunk(size_t chunk_items, size_t item_size, Chunk* previous)
		: _previous(previous)
		, _next(nullptr)
		, _size(chunk_items)
		, _end(chunk_items)
		, _free(reinterpret_cast<Item**>(this + 1))
		, _items(reinterpret_cast<char*>(_free + chunk_items))
	{
		char* raw_item = _items;
		size_t raw_item_size = Item::size(item_size);

		for (Item** i = _free; i < reinterpret_cast<Item**>(_items); ++i)
		{
			Item* item = reinterpret_cast<Item*>(raw_item);
			item->chunk = this;
			*i = item;
			raw_item += raw_item_size;
		}

		if (previous)
			previous->_next = this;
	}

	PoolBase::Chunk::~Chunk()
	{
		if (_previous)
			_previous->_next = _next;
		if (_next)
			_next->_previous = _previous;
	}

	PoolBase::Chunk::Item* PoolBase::Chunk::allocate()
	{
		return _free[--_end];
	}

	void PoolBase::Chunk::deallocate(Item* item)
	{
		_free[_end++] = item;
	}

	bool PoolBase::Chunk::is_empty() const
	{
		return _end == _size;
	}

	bool PoolBase::Chunk::is_full() const
	{
		return !_end;
	}

	size_t PoolBase::Chunk::size(size_t chunk_items, size_t item_size)
	{
		return sizeof(Chunk)
			+ chunk_items * sizeof(Item**)
			+ chunk_items * Item::size(item_size);
	}

	PoolBase::PoolBase(size_t chunk_items, size_t item_size, Allocator* allocator)
		: _allocator(allocator)
		, _chunk_items(chunk_items)
		, _item_size(item_size)
		, _chunk_size(Chunk::size(chunk_items, item_size))
		, _last_chunk(nullptr)
	{
	}

	PoolBase::~PoolBase()
	{
		for (Chunk* chunk = _last_chunk; chunk; )
		{
			Chunk* previous_chunk = _last_chunk->_previous;
			Y_DELETE(_allocator, chunk);
			chunk = previous_chunk;
		}
	}

	void* PoolBase::allocate()
	{
		if (!_last_chunk || _last_chunk->is_full())
		{
			_last_chunk = new(_allocator->allocate(_chunk_size))
				Chunk(_chunk_items, _item_size, _last_chunk);
		}

		char* pointer = _last_chunk->allocate()->data;

		return pointer;
	}

	void PoolBase::deallocate(void* pointer)
	{
		Chunk::Item* item = Chunk::Item::base(pointer);

		Chunk* chunk = item->chunk;

		chunk->deallocate(item);

		if (chunk->is_empty())
		{
			if (chunk == _last_chunk)
				_last_chunk = chunk->_previous;

			Y_DELETE(_allocator, chunk);
		}
	}

	void* PoolBase::take()
	{
		return _last_chunk ? _last_chunk->_free[_last_chunk->_end]->data : nullptr;
	}
}
