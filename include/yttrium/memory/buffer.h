/// \file
/// \brief Memory buffers.

#ifndef _include_yttrium_memory_buffer_h_
#define _include_yttrium_memory_buffer_h_

#include <yttrium/api.h>

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	/// Memory buffer.
	/// Buffer memory is allocated in whole pages making each allocation big and very aligned.
	class Y_API Buffer
	{
	public:

		/// Creates a buffer with no data.
		Buffer() noexcept = default;

		/// Creates a buffer of the specified size with uninitialized contents.
		explicit Buffer(size_t);

		///
		Buffer(size_t, const void*);

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

		///
		void reserve(size_t);

		/// Changes the buffer size without preserving its contents.
		void reset(size_t);

		/// Changes the buffer size, preserving its contents.
		/// Does not change the capacity if it is not less than the new size.
		void resize(size_t);

		/// Returns the requested size of the buffer.
		size_t size() const noexcept { return _size; }

		/// Releases unused buffer capacity if possible.
		void shrink_to_fit();

		///
		uint8_t& operator[](size_t offset) noexcept { return static_cast<uint8_t*>(_data)[offset]; }
		const uint8_t& operator[](size_t offset) const noexcept { return static_cast<const uint8_t*>(_data)[offset]; }

		/// Returns the maximum total capacity of all Buffers.
		static size_t max_total_capacity() noexcept;

		/// Returns the granularity of buffer memory, i.e. the size of a memory page.
		static size_t memory_granularity() noexcept;

		/// Returns the total capacity of all Buffers.
		static size_t total_capacity() noexcept;

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
