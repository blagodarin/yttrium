/// \file
/// \brief

#ifndef _include_yttrium_memory_buffer_appender_h_
#define _include_yttrium_memory_buffer_appender_h_

#include <yttrium/memory/buffer.h>

namespace Yttrium
{
	///
	template <typename T>
	class BufferAppender
	{
	public:
		explicit BufferAppender(Buffer& buffer) noexcept : _buffer(buffer) {}

		///
		size_t count() const noexcept { return _buffer.size() / sizeof(T); }

		///
		BufferAppender& operator<<(const T& value)
		{
			_buffer.resize(_buffer.size() + sizeof value);
			*reinterpret_cast<T*>(_buffer.end() - sizeof value) = value;
			return *this;
		}

	private:
		Buffer& _buffer;
	};
}

#endif
