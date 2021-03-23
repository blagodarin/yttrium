// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>

namespace Yt
{
	/// Memory buffer.
	/// Buffer memory is allocated in whole pages making each allocation big and very aligned.
	class Buffer
	{
	public:
		/// Creates a buffer with no data.
		constexpr Buffer() noexcept = default;

		/// Creates a buffer of the specified size with uninitialized contents.
		explicit Buffer(size_t);

		///
		Buffer(size_t, const void*);

		///
		constexpr uint8_t* begin() noexcept { return static_cast<uint8_t*>(_data); }
		constexpr const uint8_t* begin() const noexcept { return static_cast<const uint8_t*>(_data); }

		/// Returns the size of memory allocated for the buffer.
		constexpr size_t capacity() const noexcept { return _capacity; }

		///
		constexpr void clear() noexcept { _size = 0; }

		///
		constexpr void* data() noexcept { return _data; }
		constexpr const void* data() const noexcept { return _data; }

		///
		constexpr uint8_t* end() noexcept { return static_cast<uint8_t*>(_data) + _size; }
		constexpr const uint8_t* end() const noexcept { return static_cast<const uint8_t*>(_data) + _size; }

		///
		void reserve(size_t);

		/// Changes the buffer size without preserving its contents.
		void reset(size_t);

		/// Changes the buffer size, preserving its contents.
		/// Does not change the capacity if it is not less than the new size.
		void resize(size_t);

		/// Returns the requested size of the buffer.
		constexpr size_t size() const noexcept { return _size; }

		///
		bool try_reserve(size_t capacity) noexcept { return try_grow(capacity, _size, false); }

		///
		bool try_reset(size_t size) noexcept { return try_grow(size, 0, true); }

		///
		bool try_resize(size_t size) noexcept { return try_grow(size, size < _size ? size : _size, true); }

		///
		constexpr uint8_t& operator[](size_t offset) noexcept { return static_cast<uint8_t*>(_data)[offset]; }
		constexpr const uint8_t& operator[](size_t offset) const noexcept { return static_cast<const uint8_t*>(_data)[offset]; }

		/// Returns the granularity of buffer memory, i.e. the size of a memory page.
		static size_t memory_granularity() noexcept;

		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&) noexcept;
		~Buffer() noexcept;
		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) noexcept;

	private:
		bool try_grow(size_t allocate_bytes, size_t copy_bytes, bool do_resize) noexcept;

	private:
		size_t _size = 0;
		size_t _capacity = 0;
		void* _data = nullptr;
	};

	bool operator==(const Buffer&, const Buffer&) noexcept;
}
