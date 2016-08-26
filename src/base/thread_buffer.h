#ifndef _src_base_thread_buffer_h_
#define _src_base_thread_buffer_h_

#include <yttrium/timer.h>

#include <condition_variable>
#include <mutex>

namespace Yttrium
{
	class ThreadBufferBase
	{
	public:

		ThreadBufferBase(size_t capacity) : _capacity(capacity) {}

		ThreadBufferBase(const ThreadBufferBase&) = delete;
		ThreadBufferBase& operator=(const ThreadBufferBase&) = delete;

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

		const size_t _capacity;
		size_t _size = 0;
		size_t _first = 0;

	private:

		std::mutex              _mutex;
		std::condition_variable _read;
		std::condition_variable _write;
	};

	template <typename T, size_t N = 1>
	class ThreadBuffer : public ThreadBufferBase
	{
	public:

		ThreadBuffer() : ThreadBufferBase(N) {}

		T read()
		{
			begin_read();
			const T& result = _buffer[_first];
			end_read();
			return result;
		}

		T read(Clock milliseconds, const T& default_value = T())
		{
			if (!try_begin_read(milliseconds))
				return default_value;
			const T& result = _buffer[_first];
			end_read();
			return result;
		}

		bool try_read(T* value)
		{
			if (!try_begin_read())
				return false;
			*value = _buffer[_first];
			end_read();
			return true;
		}

		bool try_read(T* value, Clock milliseconds)
		{
			if (!try_begin_read(milliseconds))
				return false;
			*value = _buffer[_first];
			end_read();
			return true;
		}

		bool try_write(const T& value)
		{
			if (!try_begin_write())
				return false;
			_buffer[(_first + _size) % _capacity] = value;
			end_write();
			return true;
		}

		bool try_write(const T& value, Clock milliseconds)
		{
			if (!try_begin_write(milliseconds))
				return false;
			_buffer[(_first + _size) % _capacity] = value;
			end_write();
			return true;
		}

		void write(const T& value)
		{
			begin_write();
			_buffer[(_first + _size) % _capacity] = value;
			end_write();
		}

	private:

		T _buffer[N];
	};
}

#endif
