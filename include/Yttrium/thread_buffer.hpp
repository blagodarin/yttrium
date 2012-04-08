/// \file
/// \brief Multithreading buffer.

#ifndef __Y_THREAD_BUFFER_HPP
#define __Y_THREAD_BUFFER_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/time.hpp>

namespace Yttrium
{

class Y_API ThreadBufferBase
{
public:

	ThreadBufferBase(size_t capacity, Allocator *allocator = HeapAllocator::instance()) noexcept;

	~ThreadBufferBase() noexcept;

	ThreadBufferBase(const ThreadBufferBase &) = delete;

	ThreadBufferBase &operator =(const ThreadBufferBase &) = delete;

protected:

	void begin_read();

	void begin_write();

	void end_read();

	void end_write();

	bool try_begin_read();

	bool try_begin_read(Clock milliseconds);

	bool try_begin_write();

	bool try_begin_write(Clock milliseconds);

protected:

	size_t _capacity;
	size_t _size;
	size_t _first;

public:

	class Private;

private:

	Private *_private;
};

/// Cross-thread communication buffer.
/// \tparam T Element type.
/// \tparam N Buffer size.

template <typename T, size_t N = 1>
class ThreadBuffer: public ThreadBufferBase
{
public:

	ThreadBuffer(Allocator *allocator = HeapAllocator::instance()) noexcept
		: ThreadBufferBase(N, allocator)
	{
	}

public:

	///

	T read() noexcept
	{
		begin_read();
		const T &result = _buffer[_first];
		end_read();
		return result;
	}

	///

	T read(Clock milliseconds, const T &default_value = T()) noexcept
	{
		if (!try_begin_read(milliseconds))
		{
			return default_value;
		}
		const T &result = _buffer[_first];
		end_read();
		return result;
	}

	///

	bool try_read(T *value) noexcept
	{
		if (!try_begin_read())
		{
			return false;
		}
		*value = _buffer[_first];
		end_read();
		return true;
	}

	///

	bool try_read(T *value, Clock milliseconds) noexcept
	{
		if (!try_begin_read(milliseconds))
		{
			return false;
		}
		*value = _buffer[_first];
		end_read();
		return true;
	}

	///

	bool try_write(const T &value) noexcept
	{
		if (!try_begin_write())
		{
			return false;
		}
		_buffer[(_first + _size) % _capacity] = value;
		end_write();
		return true;
	}

	///

	bool try_write(const T &value, Clock milliseconds) noexcept
	{
		if (!try_begin_write(milliseconds))
		{
			return false;
		}
		_buffer[(_first + _size) % _capacity] = value;
		end_write();
		return true;
	}

	///

	void write(const T &value) noexcept
	{
		begin_write();
		_buffer[(_first + _size) % _capacity] = value;
		end_write();
	}

private:

	T _buffer[N];
};

} // namespace Yttrium

#endif // __Y_THREAD_BUFFER_HPP
