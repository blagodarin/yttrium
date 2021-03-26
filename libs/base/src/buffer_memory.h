// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

namespace Yt
{
	class BufferMemory
	{
	public:
		// Internal properties required for proper benchmarking.
		constexpr static size_t MaxSmallBlockLevel = 19;
		constexpr static size_t MaxSmallBlockSize = 1 << MaxSmallBlockLevel;

		BufferMemory() = default;

		void* allocate(size_t capacity) noexcept;
		void deallocate(void* data, size_t capacity) noexcept;
		void* reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size) noexcept;

		static size_t capacity_for_size(size_t) noexcept;
		static size_t granularity() noexcept;
		static size_t total_capacity() noexcept;

		BufferMemory(const BufferMemory&) = delete;
		BufferMemory& operator=(const BufferMemory&) = delete;

	private:
		void* _small_blocks[MaxSmallBlockLevel + 1]{};
		std::mutex _small_blocks_mutex;
	};

	extern BufferMemory _buffer_memory; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
}
