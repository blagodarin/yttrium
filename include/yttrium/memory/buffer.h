/// \file
/// \brief Memory buffers.

#ifndef _include_yttrium_memory_buffer_h_
#define _include_yttrium_memory_buffer_h_

#include <yttrium/global.h>

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	/// Memory buffer.
	/// Buffer memory is allocated in whole pages making each allocation big and very aligned.
	/// Memory buffers can be used without MemoryManager.
	class Y_API Buffer
	{
	public:

		/// Creates a buffer with no data.
		Buffer() noexcept = default;

		/// Creates a buffer of the specified size with uninitialized contents.
		explicit Buffer(size_t);

		///
		uint8_t* begin() noexcept { return static_cast<uint8_t*>(_data); }
		const uint8_t* begin() const noexcept { return static_cast<const uint8_t*>(_data); }

		/// Returns the size of memory allocated for the buffer.
		size_t capacity() const noexcept { return _capacity; }

		///
		void* data() noexcept { return _data; }
		const void* data() const noexcept { return _data; }

		///
		uint8_t* end() noexcept { return static_cast<uint8_t*>(_data) + _size; }
		const uint8_t* end() const noexcept { return static_cast<const uint8_t*>(_data) + _size; }

		/// Changes the buffer size, invalidating its contents.
		/// The buffer capacity may remain unchanged if the specified size is not greater than current capacity.
		void reset(size_t);

		/// Returns the requested size of the buffer.
		size_t size() const noexcept { return _size; }

		/// Releases unused buffer capacity if possible.
		void shrink_to_fit() noexcept;

		///
		uint8_t& operator[](size_t offset) noexcept { return static_cast<uint8_t*>(_data)[offset]; }
		const uint8_t& operator[](size_t offset) const noexcept { return static_cast<const uint8_t*>(_data)[offset]; }

		/// Returns the maximum size of all buffer memory allocated at one point of time since startup.
		static size_t max_total_memory_allocated() noexcept;

		/// Returns the granularity of buffer memory, i.e. the size of a memory page.
		static size_t memory_granularity() noexcept;

		/// Returns the total size of all currently allocated buffer memory.
		static size_t total_memory_allocated() noexcept;

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&) noexcept;
		~Buffer() noexcept;
		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) noexcept;
	private:
		size_t _size = 0;
		size_t _capacity = 0;
		void* _data = nullptr;
	};

	Y_API bool operator==(const Buffer&, const Buffer&) noexcept;
	inline bool operator!=(const Buffer& lhs, const Buffer& rhs) noexcept { return !(lhs == rhs); }
}

#endif
