#include <yttrium/string.h>

#include <yttrium/allocator.h>
#include <yttrium/utils.h>

#include <cassert>
#include <cstring>

namespace Yttrium
{
	const size_t StringReserve   =  16; // Minimal memory block size allocated by the String.
	const size_t StringGrowBound = 256; // The boundary until String size will double.
	const size_t StringGrowStep  = 256; // Linear String grow step after the boundary.

	struct String::Private
	{
		static void deallocate(String& string)
		{
			if (string._buffer_size == 0)
				return;
			const auto references = reinterpret_cast<size_t*>(string.text()) - 1;
			if (!--*references)
				string._allocator->deallocate(references);
		}
	};

	String::String(const String& string)
		: StaticString(string._size)
		, _buffer_size(max(_size + 1, StringReserve))
		, _allocator(string._allocator)
	{
		init(string._text, _size);
	}

	String::String(String&& string) noexcept
		: StaticString(string._text, string._size)
		, _buffer_size(string._buffer_size)
		, _allocator(string._allocator)
	{
		string._buffer_size = 0;
	}

	String::String(const StaticString& string, Allocator* allocator)
		: StaticString(string.size())
		, _buffer_size(max(_size + 1, StringReserve))
		, _allocator(allocator)
	{
		init(string.text(), _size);
	}

	String::String(const char* text, Allocator* allocator)
		: StaticString(::strlen(text))
		, _buffer_size(max(_size + 1, StringReserve))
		, _allocator(allocator)
	{
		init(text, _size);
	}

	String::String(size_t size, Allocator* allocator)
		: _buffer_size(max(size, StringReserve))
		, _allocator(allocator)
	{
		init(&StringNull, 0);
	}

	String::~String()
	{
		Private::deallocate(*this);
	}

	String& String::clear() noexcept
	{
		if (_buffer_size > 0)
		{
			size_t* references = reinterpret_cast<size_t*>(const_cast<char*>(_text)) - 1;
			if (*references == 1)
			{
				const_cast<char*>(_text)[0] = '\0';
				_size = 0;
				return *this;
			}
			--*references;
			_buffer_size = 0;
		}
		_text = &StringNull;
		_size = 0;
		return *this;
	}

	void String::insert(const StaticString& text, size_t index)
	{
		if (index > _size)
			return;

		const size_t buffer_size = _size + text.size() + 1;

		if (_buffer_size > 0)
		{
			grow(buffer_size);
			if (index < _size)
				::memmove(const_cast<char*>(_text) + index + text.size(), _text + index, _size - index);
		}
		else
		{
			const char* old_text = init(buffer_size);
			if (index > 0)
				::memcpy(const_cast<char*>(_text), old_text, index);
			if (index < _size)
				::memcpy(const_cast<char*>(_text) + index + text.size(), old_text + index, _size - index);
		}

		::memcpy(const_cast<char*>(_text) + index, text.text(), text.size());
		_size += text.size();
		const_cast<char*>(_text)[_size] = '\0';
	}

	void String::insert(char symbol, size_t index)
	{
		if (index > _size)
			return;

		const size_t buffer_size = _size + 2;

		if (_buffer_size > 0)
		{
			grow(buffer_size);
			if (index < _size)
				::memmove(const_cast<char*>(_text) + index + 1, _text + index, _size - index);
		}
		else
		{
			const char* old_text = init(buffer_size);
			if (index)
				::memcpy(const_cast<char*>(_text), old_text, index);
			if (index < _size)
				::memcpy(const_cast<char*>(_text) + index + 1, old_text + index, _size - index);
		}

		const_cast<char*>(_text)[index] = symbol;
		++_size;
		const_cast<char*>(_text)[_size] = '\0';
	}

	void String::remove(size_t index, size_t size)
	{
		if (!size || index >= _size)
			return;

		size = min(size, _size - index);

		size_t* references = nullptr;

		if (_buffer_size > 0)
		{
			references = reinterpret_cast<size_t*>(const_cast<char*>(_text)) - 1;
			if (*references == 1)
			{
				_size -= size;
				::memmove(const_cast<char*>(_text) + index, _text + index + size, _size - index);
				const_cast<char*>(_text)[_size] = '\0';
				return;
			}
		}

		const char* old_text = init(size + 1);

		if (index)
			::memcpy(const_cast<char*>(_text), old_text, index);

		_size -= size;
		::memcpy(const_cast<char*>(_text) + index, old_text + index + size, _size - index);
		const_cast<char*>(_text)[_size] = '\0';

		if (references)
			--*references;
	}

	void String::reserve(size_t size)
	{
		size_t buffer_size = size + 1;
		if (_buffer_size > 0)
		{
			grow(buffer_size);
		}
		else
		{
			const char* old_text = init(max(_size + 1, buffer_size));
			::memcpy(const_cast<char*>(_text), old_text, _size + 1);
			const_cast<char*>(_text)[_size] = '\0';
		}
	}

	void String::resize(size_t size)
	{
		const size_t buffer_size = size + 1;
		if (_buffer_size > 0)
		{
			grow(buffer_size);
		}
		else
		{
			const char* old_text = init(max(_size + 1, buffer_size));
			::memcpy(const_cast<char*>(_text), old_text, _size + 1);
		}

		const_cast<char*>(_text)[size] = '\0';
		_size = size;
	}

	void String::truncate(size_t size)
	{
		if (size >= _size)
			return;

		size_t* references = nullptr;

		if (_buffer_size > 0)
		{
			references = reinterpret_cast<size_t*>(const_cast<char*>(_text)) - 1;
			if (*references == 1)
			{
				const_cast<char*>(_text)[size] = '\0';
				_size = size;
				return;
			}
		}

		const char* old_text = init(size + 1);
		::memcpy(const_cast<char*>(_text), old_text, size);
		const_cast<char*>(_text)[size] = '\0';
		_size = size;

		if (references)
			--*references;
	}

	String& String::swap(String* string) noexcept
	{
		const auto text = _text;
		const auto size = _size;
		const auto buffer_size = _buffer_size;
		const auto allocator = _allocator;

		_text = string->_text;
		_size = string->_size;
		_buffer_size = string->_buffer_size;
		_allocator = string->_allocator;

		string->_text = text;
		string->_size = size;
		string->_buffer_size = buffer_size;
		string->_allocator = allocator;

		return *this;
	}

	String& String::swap(String&& string) noexcept
	{
		const auto text = _text;
		const auto size = _size;
		const auto buffer_size = _buffer_size;
		const auto allocator = _allocator;

		_text = string._text;
		_size = string._size;
		_buffer_size = string._buffer_size;
		_allocator = string._allocator;

		string._text = text;
		string._size = size;
		string._buffer_size = buffer_size;
		string._allocator = allocator;

		return *this;
	}

	String& String::trim()
	{
		const StaticString& trimmed_string = this->trimmed();
		if (trimmed_string.size() < _size)
		{
			const size_t buffer_size = trimmed_string.size() + 1;
			if (_buffer_size > 0)
				grow(buffer_size);
			else
				init(buffer_size);

			::memmove(const_cast<char*>(_text), trimmed_string.text(), trimmed_string.size());
			const_cast<char*>(_text)[trimmed_string.size()] = '\0';
			_size = trimmed_string.size();
		}
		return *this;
	}

	String& String::operator=(const StaticString& string)
	{
		const auto buffer_size = string.size() + 1;
		if (_buffer_size > 0)
			grow(buffer_size);
		else
			init(buffer_size);

		::memcpy(const_cast<char*>(_text), string.text(), string.size());
		const_cast<char*>(_text)[string.size()] = '\0';
		_size = string.size();

		return *this;
	}

	String& String::operator=(String&& string) noexcept
	{
		Private::deallocate(*this);
		_text = string._text;
		_size = string._size;
		_buffer_size = string._buffer_size;
		_allocator = string._allocator;
		string._buffer_size = 0;
		return *this;
	}

	void String::grow(size_t buffer_size)
	{
		assert(_buffer_size > 0);

		size_t* references = reinterpret_cast<size_t*>(const_cast<char*>(_text)) - 1;
		if (*references != 1)
		{
			const char* old_text = _text;
			init(buffer_size);
			::memcpy(const_cast<char*>(_text), old_text, _size);
			const_cast<char*>(_text)[_size] = '\0';
			--*references;
		}
		else if (_buffer_size < buffer_size)
		{
			size_t adjusted_size = max(buffer_size,
				(_buffer_size < StringGrowBound
					? _buffer_size * 2
					: _buffer_size + StringGrowStep));

			size_t* pointer = static_cast<size_t*>(
				_allocator->reallocate(
					reinterpret_cast<size_t*>(const_cast<char*>(_text)) - 1,
					sizeof(size_t) + adjusted_size));

			_text = reinterpret_cast<char*>(pointer + 1);
			_buffer_size = adjusted_size;
		}
	}

	void String::init()
	{
		assert(_buffer_size > 0);
		assert(_allocator);

		size_t* pointer = static_cast<size_t*>(_allocator->allocate(sizeof(size_t) + _buffer_size));
		*pointer = 1;
		_text = reinterpret_cast<char*>(pointer + 1);
	}

	void String::init(const char* string, size_t size)
	{
		init();
		::memcpy(const_cast<char*>(_text), string, size);
		const_cast<char*>(_text)[size] = '\0';
	}

	const char* String::init(size_t buffer_size)
	{
		assert(_buffer_size == 0);

		if (!_allocator) // Rely on this at your own risk.
			return _text;

		const size_t adjusted_size = max(buffer_size, StringReserve);
		size_t* pointer = static_cast<size_t*>(_allocator->allocate(sizeof(size_t) + adjusted_size));
		*pointer = 1;
		const char* old_text = _text;
		_text = reinterpret_cast<char*>(pointer + 1);
		_buffer_size = adjusted_size;
		return old_text;
	}
}
