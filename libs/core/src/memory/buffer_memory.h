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

#include <mutex>

namespace Yttrium
{
	class BufferMemory
	{
	public:
		// Internal properties required for proper benchmarking.
		constexpr static size_t MaxSmallBlockLevel = 19;
		constexpr static size_t MaxSmallBlockSize = 1 << MaxSmallBlockLevel;

		BufferMemory() = default;
		~BufferMemory();

		void* allocate(size_t capacity);
		void deallocate(void* data, size_t capacity) noexcept;
		void* reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size);

		static size_t capacity_for_size(size_t) noexcept;
		static size_t granularity() noexcept;
		static size_t total_capacity() noexcept;

		BufferMemory(const BufferMemory&) = delete;
		BufferMemory& operator=(const BufferMemory&) = delete;

	private:
		void* _small_blocks[MaxSmallBlockLevel + 1] = {};
		std::mutex _small_blocks_mutex;
	};

	extern BufferMemory _buffer_memory;
}
