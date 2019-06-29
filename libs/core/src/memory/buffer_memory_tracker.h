//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#pragma once

#if !Y_ENABLE_BUFFER_MEMORY_TRACKING
#	error Buffer memory tracking is disabled.
#endif

#include <yttrium/api.h>
#include "../utils/atomic_counters.h"

#include <cstddef>

#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
#	include <map>
#endif

namespace Yttrium
{
	class BufferMemoryTracker
	{
	public:
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
		void print_state(const std::map<size_t, size_t>& free_block_count);
#endif

		size_t total_capacity() const noexcept
		{
			return _total_capacity.current_value();
		}

		void track_capacity_allocation(size_t capacity) noexcept
		{
			_total_capacity += capacity;
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
			++_allocations;
			++_total_allocations;
#endif
		}

		void track_capacity_change(size_t old_capacity, size_t new_capacity) noexcept
		{
			if (new_capacity > old_capacity)
				_total_capacity += new_capacity - old_capacity;
			else
				_total_capacity -= old_capacity - new_capacity;
		}

		void track_capacity_deallocation(size_t capacity) noexcept
		{
			_total_capacity -= capacity;
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
			--_allocations;
#endif
		}

		void track_reallocation() noexcept
		{
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
			++_total_reallocations;
#endif
		}

		void track_system_allocation(size_t size) noexcept
		{
			_total_allocated += size;
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
			++_total_system_allocations;
#endif
		}

		void track_system_deallocation(size_t size) noexcept
		{
			_total_allocated -= size;
		}

		void track_system_reallocation(size_t old_size, size_t new_size) noexcept
		{
			if (new_size > old_size)
				_total_allocated += new_size - old_size;
			else
				_total_allocated -= old_size - new_size;
		}

	private:
		MaxCounter<size_t> _total_allocated;
		MaxCounter<size_t> _total_capacity;
#if Y_ENABLE_BUFFER_MEMORY_DEBUGGING
		MaxCounter<size_t> _allocations;
		std::atomic<size_t> _total_system_allocations{ 0 };
		std::atomic<size_t> _total_allocations{ 0 };
		std::atomic<size_t> _total_reallocations{ 0 };
#endif
	};

	extern Y_CORE_API BufferMemoryTracker _buffer_memory_tracker;
}
