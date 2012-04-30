#include "pool.hpp"

namespace Yttrium
{

PoolBase::Private::~Private()
{
	for (Chunk *chunk = _last_chunk; chunk; )
	{
		Chunk *previous_chunk = _last_chunk->_previous;
		_allocator->delete_(chunk);
		chunk = previous_chunk;
	}
}

char *PoolBase::Private::allocate()
{
	if (!_last_chunk || _last_chunk->is_full())
	{
		_last_chunk = new(_allocator->allocate(_chunk_size))
			Chunk(_item_size, _chunk_items, _last_chunk);

		++_status.allocated_chunks;
		++_status.chunk_allocations;
	}

	char *pointer = _last_chunk->allocate()->data;

	++_status.allocated_items;
	++_status.item_allocations;

	return pointer;
}

void PoolBase::Private::deallocate(char *pointer)
{
	Chunk::Item *item = Chunk::Item::base(pointer);

	Chunk *chunk = item->chunk;

	chunk->deallocate(item);

	--_status.allocated_items;
	++_status.item_deallocations;

	if (chunk->is_empty())
	{
		if (chunk == _last_chunk)
		{
			_last_chunk = chunk->_previous;
		}

		_allocator->delete_(chunk);

		--_status.allocated_chunks;
		++_status.chunk_deallocations;
	}
}

char *PoolBase::Private::take()
{
	return _last_chunk
		? _last_chunk->_free[_last_chunk->_end]->data
		: nullptr;
}

PoolBase::PoolBase(size_t item_size, size_t chunk_size, Allocator *allocator)
	: _private(new(allocator->allocate<Private>())
		Private(item_size, chunk_size, allocator))
{
}

PoolBase::~PoolBase()
{
	_private->_allocator->delete_(_private);
}

PoolStatus PoolBase::status() const
{
	return _private->_status;
}

void *PoolBase::allocate()
{
	return _private->allocate();
}

void PoolBase::deallocate(void *pointer)
{
	return _private->deallocate(static_cast<char *>(pointer));
}

void *PoolBase::take()
{
	return _private->take();
}

} // namespace Yttrium
