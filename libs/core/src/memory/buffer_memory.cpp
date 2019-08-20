//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include "buffer_memory.h"

#include <yttrium/utils/numeric.h>
#include "../platform/virtual_memory.h"

#include <cassert>
#include <cstddef>
#include <cstring>

namespace
{
	size_t level_from_capacity(size_t capacity) noexcept
	{
		assert(Yt::is_power_of_2(capacity));
		size_t level = 0;
		for (auto i = capacity; i > 1; i >>= 1)
			++level;
		assert(size_t{ 1 } << level == capacity);
		return level;
	}

	struct Block
	{
		void* _next;
	};
}

namespace Yt
{
	BufferMemory::~BufferMemory() noexcept = default;

	void* BufferMemory::allocate(size_t capacity) noexcept
	{
		assert(capacity > 0 && capacity == capacity_for_size(capacity));
		if (capacity > MaxSmallBlockSize)
			return vm_allocate(capacity);
		const auto matching_level = ::level_from_capacity(capacity);
		std::scoped_lock lock{ _small_blocks_mutex };
		if (const auto matching_block = _small_blocks[matching_level])
		{
			_small_blocks[matching_level] = static_cast<Block*>(matching_block)->_next;
			return matching_block;
		}
		auto block_level = matching_level + 1;
		void* block = nullptr;
		for (; block_level <= MaxSmallBlockLevel; ++block_level)
		{
			if (block = _small_blocks[block_level]; block)
			{
				_small_blocks[block_level] = static_cast<Block*>(block)->_next;
				break;
			}
		}
		if (!block)
			if (block = vm_allocate(2 * MaxSmallBlockSize); !block) // TODO: Try merging smaller blocks before allocating a new big one.
				return nullptr;
		do
		{
			--block_level;
			static_cast<Block*>(block)->_next = _small_blocks[block_level];
			_small_blocks[block_level] = block;
			block = static_cast<std::byte*>(block) + (size_t{ 1 } << block_level);
		} while (block_level > matching_level);
		return block;
	}

	void BufferMemory::deallocate(void* data, size_t capacity) noexcept
	{
		assert(data);
		assert(capacity > 0 && capacity == capacity_for_size(capacity));
		if (capacity > MaxSmallBlockSize)
			return vm_deallocate(data, capacity);
		const auto level = ::level_from_capacity(capacity);
		std::scoped_lock lock{ _small_blocks_mutex };
		static_cast<Block*>(data)->_next = _small_blocks[level];
		_small_blocks[level] = data;
	}

	void* BufferMemory::reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size) noexcept
	{
		assert(old_data);
		assert(old_capacity > 0 && old_capacity == capacity_for_size(old_capacity));
		assert(new_capacity > 0 && new_capacity == capacity_for_size(new_capacity));
		assert(old_capacity != new_capacity);
		if (old_capacity > MaxSmallBlockSize && new_capacity > MaxSmallBlockSize)
			return vm_reallocate(old_data, old_capacity, new_capacity);
		const auto new_data = allocate(new_capacity);
		if (!new_data)
			return nullptr;
		if (old_size > 0)
			::memcpy(new_data, old_data, old_size);
		deallocate(old_data, old_capacity);
		return new_data;
	}

	size_t BufferMemory::capacity_for_size(size_t size) noexcept
	{
		const auto granularity_mask = granularity() - 1;
		auto capacity = (size + granularity_mask) & ~granularity_mask;
		return capacity > MaxSmallBlockSize ? capacity : next_power_of_2(capacity);
	}

	size_t BufferMemory::granularity() noexcept
	{
		static const auto page_size = vm_granularity();
		assert(is_power_of_2(page_size));
		return page_size;
	}
}
