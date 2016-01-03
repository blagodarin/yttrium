#include <yttrium/memory/buffer.h>

#include "pages.h"

#include <atomic>
#include <cstring>
#include <new>

#if Y_IS_DEBUG
	#include <iostream>
#endif

namespace Yttrium
{
	namespace
	{
		struct BufferMemoryStatus
		{
			std::atomic<size_t> _allocated{0};
			std::atomic<size_t> _max_allocated{0};

			void on_allocated(size_t size)
			{
				auto old_allocated = _allocated.fetch_add(size);
				const auto new_allocated = old_allocated + size;
				while (!_max_allocated.compare_exchange_strong(old_allocated, new_allocated))
					if (old_allocated >= new_allocated)
						break;
			}

			void on_deallocated(size_t size)
			{
				_allocated.fetch_sub(size);
			}

		#if Y_IS_DEBUG
			~BufferMemoryStatus()
			{
				const auto max_allocated = _max_allocated.load();
				std::cerr << "(DEBUG) Max buffer memory allocated: " << max_allocated / 1024 << " KiB" << std::endl;
				const auto allocated = _allocated.load();
				if (allocated > 0)
					std::cerr << "(ERROR) Buffer memory leaked: " << allocated / 1024 << " KiB" << std::endl;
			}
		#endif
		};

		BufferMemoryStatus _buffer_memory_status;
	}

	Buffer::Buffer(size_t size)
		: _size(size)
		, _capacity(pages_size(_size))
		, _data(pages_allocate(_capacity))
	{
		if (!_data)
			throw std::bad_alloc();
		_buffer_memory_status.on_allocated(_capacity);
	}

	void Buffer::reset(size_t size)
	{
		if (size > _capacity)
		{
			const auto new_capacity = pages_size(size);
			const auto new_data = _data ? pages_reallocate(_data, _capacity, new_capacity) : pages_allocate(new_capacity);
			if (!new_data)
				throw std::bad_alloc();
			_buffer_memory_status.on_allocated(new_capacity - _capacity);
			_capacity = new_capacity;
			_data = new_data;
		}
		_size = size;
	}

	size_t Buffer::max_memory_allocated() noexcept
	{
		return _buffer_memory_status._max_allocated;
	}

	size_t Buffer::memory_granularity() noexcept
	{
		return pages_size(1);
	}

	Buffer::Buffer(Buffer&& other) noexcept
		: _size(other._size)
		, _capacity(other._capacity)
		, _data(other._data)
	{
		other._data = nullptr;
	}

	Buffer::~Buffer() noexcept
	{
		if (_data)
		{
			pages_deallocate(_data, _capacity);
			_buffer_memory_status.on_deallocated(_capacity);
		}
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		if (_data)
		{
			pages_deallocate(_data, _capacity);
			_buffer_memory_status.on_deallocated(_capacity);
		}
		_size = other._size;
		_capacity = other._capacity;
		_data = other._data;
		other._data = nullptr;
		return *this;
	}

	bool operator==(const Buffer& lhs, const Buffer& rhs) noexcept
	{
		return lhs.size() == rhs.size() && (lhs.size() == 0 || !::memcmp(lhs.data(), rhs.data(), lhs.size()));
	}
}
