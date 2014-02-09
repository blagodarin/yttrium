#ifndef __BASE_THREAD_BUFFER_H
#define __BASE_THREAD_BUFFER_H

#include <yttrium/time.h>

#include <condition_variable>
#include <mutex>

namespace Yttrium
{

class ThreadBufferBase
{
	Y_NONCOPYABLE(ThreadBufferBase);

public:

	ThreadBufferBase(size_t capacity) noexcept;

	~ThreadBufferBase() noexcept;

protected:

	void begin_read() noexcept;

	void begin_write() noexcept;

	void end_read() noexcept;

	void end_write() noexcept;

	bool try_begin_read() noexcept;

	bool try_begin_read(Clock milliseconds) noexcept;

	bool try_begin_write() noexcept;

	bool try_begin_write(Clock milliseconds) noexcept;

protected:

	size_t _capacity;
	size_t _size;
	size_t _first;

private:

	std::mutex              _mutex;
	std::condition_variable _read;
	std::condition_variable _write;
};

template <typename T, size_t N = 1>
class ThreadBuffer
	: public ThreadBufferBase
{
public:

	ThreadBuffer() noexcept
		: ThreadBufferBase(N)
	{
	}

public:

	T read() noexcept
	{
		begin_read();
		const T &result = _buffer[_first];
		end_read();
		return result;
	}

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

#endif // __BASE_THREAD_BUFFER_H
