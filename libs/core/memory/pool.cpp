//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/memory/pool.h>

#include "raw.h"

// TODO: Get rid of magical chunk item count in places where pools are used.
// Perhaps setting the pool chunk size to memory page size will do.

namespace Yttrium
{
	class PoolChunk
	{
	public:
		struct Item
		{
			PoolChunk* chunk;
			char data[1];

			static Item* base(void* pointer)
			{
				return reinterpret_cast<Item*>(static_cast<char*>(pointer) - offsetof(Item, data));
			}

			static size_t size(size_t item_size)
			{
				return offsetof(Item, data) + item_size;
			}
		};

		PoolChunk(size_t chunk_items, size_t item_size, PoolChunk* previous)
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

		~PoolChunk()
		{
			if (_previous)
				_previous->_next = _next;
			if (_next)
				_next->_previous = _previous;
		}

		Item* allocate()
		{
			return _free[--_end];
		}

		void deallocate(Item* item)
		{
			_free[_end++] = item;
		}

		bool is_empty() const
		{
			return _end == _size;
		}

		bool is_full() const
		{
			return !_end;
		}

		static size_t size(size_t chunk_items, size_t item_size)
		{
			return sizeof(PoolChunk)
				+ chunk_items * sizeof(Item**)
				+ chunk_items * Item::size(item_size);
		}

	public:
		PoolChunk* _previous;
		PoolChunk* _next;
		size_t _size;
		size_t _end;
		Item** _free;
		char* _items;
	};

	// cppcheck-suppress uninitMemberVar
	PoolBase::PoolBase(size_t chunk_items, size_t item_size) noexcept
		: _chunk_items{ chunk_items }
		, _item_size{ item_size }
		, _chunk_size{ PoolChunk::size(_chunk_items, _item_size) }
	{
	}

	PoolBase::~PoolBase()
	{
		for (auto chunk = _last_chunk; chunk;)
		{
			const auto previous_chunk = _last_chunk->_previous;
			delete_sized(chunk);
			chunk = previous_chunk;
		}
	}

	void* PoolBase::allocate()
	{
		if (!_last_chunk || _last_chunk->is_full())
			_last_chunk = new_sized<PoolChunk>(_chunk_size, _chunk_items, _item_size, _last_chunk);

		char* pointer = _last_chunk->allocate()->data;

		return pointer;
	}

	void PoolBase::deallocate(void* pointer) noexcept
	{
		const auto item = PoolChunk::Item::base(pointer);

		const auto chunk = item->chunk;

		chunk->deallocate(item);

		if (chunk->is_empty())
		{
			if (chunk == _last_chunk)
				_last_chunk = chunk->_previous;

			delete_sized(chunk);
		}
	}

	void* PoolBase::take() noexcept
	{
		return _last_chunk ? _last_chunk->_free[_last_chunk->_end]->data : nullptr;
	}
}
