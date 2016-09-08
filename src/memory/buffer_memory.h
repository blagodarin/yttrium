#ifndef _src_memory_buffer_memory_h_
#define _src_memory_buffer_memory_h_

#include <mutex>

namespace Yttrium
{
	class NamedAllocatorData;

	class BufferMemory
	{
	public:
		// Internal properties required for proper benchmarking.
		constexpr static size_t MaxSmallBlockLevel = 19;
		constexpr static size_t MaxSmallBlockSize = 1 << MaxSmallBlockLevel;

		BufferMemory();
		~BufferMemory();

		void* allocate(size_t capacity);
		void deallocate(void* data, size_t capacity) noexcept;
		void* reallocate(void* old_data, size_t old_capacity, size_t new_capacity, size_t old_size);

		static size_t capacity_for_size(size_t) noexcept;
		static size_t granularity() noexcept;
		static size_t max_total_capacity() noexcept;
		static size_t total_capacity() noexcept;

		BufferMemory(const BufferMemory&) = delete;
		BufferMemory& operator=(const BufferMemory&) = delete;

	private:
		NamedAllocatorData* const _named_allocator_data;
		void* _small_blocks[MaxSmallBlockLevel + 1] = {};
		std::mutex _small_blocks_mutex;
	};

	extern BufferMemory _buffer_memory;
}

#endif
