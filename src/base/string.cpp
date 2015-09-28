#include <yttrium/string.h>

#include <yttrium/allocator.h>
#include <yttrium/utils.h>

#include <cassert>
#include <cstring>

namespace Yttrium
{
	struct String::Private
	{
		static constexpr size_t MinCapacity = 16;

		static void grow(String& s, size_t capacity)
		{
			assert(s._capacity > 0);
			if (capacity <= s._capacity)
				return;
			const auto old_text = const_cast<char*>(s._text);
			const auto new_capacity = max(capacity, next_power_of_2(s._capacity + 1));
			const auto new_text = static_cast<char*>(s._allocator->allocate(new_capacity));
			::memcpy(new_text, old_text, s._size); // Don't copy the '\0', it will be written to the string later.
			s._allocator->deallocate(old_text, true);
			s._text = new_text;
			s._capacity = new_capacity;
		}

		static void initialize(String& s, size_t capacity)
		{
			assert(s._capacity == 0);
			if (!s._allocator)
				return; // Rely on this at your own risk.
			const auto actual_capacity = max(capacity, MinCapacity);
			s._text = static_cast<char*>(s._allocator->allocate(actual_capacity));
			s._capacity = actual_capacity;
		}

		static void initialize_copy(String& s, const StaticString& source)
		{
			assert(!s._text);
			assert(s._size == source.size());
			assert(s._capacity > 0);
			assert(s._allocator);
			const auto text = static_cast<char*>(s._allocator->allocate(s._capacity));
			::memcpy(text, source.text(), source.size());
			text[source.size()] = '\0';
			s._text = text;
		}
	};

	String::String(const String& string)
		: StaticString(nullptr, string._size)
		, _capacity(max(_size + 1, Private::MinCapacity))
		, _allocator(string._allocator)
	{
		Private::initialize_copy(*this, string);
	}

	String::String(String&& string) noexcept
		: StaticString(string._text, string._size)
		, _capacity(string._capacity)
		, _allocator(string._allocator)
	{
		string._capacity = 0;
	}

	String::String(const StaticString& string, Allocator* allocator)
		: StaticString(nullptr, string.size())
		, _capacity(max(_size + 1, Private::MinCapacity))
		, _allocator(allocator)
	{
		Private::initialize_copy(*this, string);
	}

	String::String(size_t size, Allocator* allocator)
		: StaticString(nullptr, 0)
		, _capacity(max(size + 1, Private::MinCapacity))
		, _allocator(allocator)
	{
		Private::initialize_copy(*this, {});
	}

	String::~String()
	{
		if (_capacity > 0)
			_allocator->deallocate(const_cast<char*>(_text));
	}

	String& String::clear() noexcept
	{
		if (_capacity > 0)
			const_cast<char*>(_text)[0] = '\0';
		else
			_text = &StringNull;
		_size = 0;
		return *this;
	}

	void String::insert(const StaticString& text, size_t offset)
	{
		if (offset > _size)
			return;

		const size_t capacity = _size + text.size() + 1;

		if (_capacity > 0)
		{
			Private::grow(*this, capacity);
			if (offset < _size)
				::memmove(const_cast<char*>(_text) + offset + text.size(), _text + offset, _size - offset);
		}
		else
		{
			const auto old_text = _text;
			Private::initialize(*this, capacity);
			if (offset > 0)
				::memcpy(const_cast<char*>(_text), old_text, offset);
			if (offset < _size)
				::memcpy(const_cast<char*>(_text) + offset + text.size(), old_text + offset, _size - offset);
		}

		::memcpy(const_cast<char*>(_text) + offset, text.text(), text.size());
		_size += text.size();
		const_cast<char*>(_text)[_size] = '\0';
	}

	void String::remove(size_t offset, size_t size)
	{
		if (!size || offset >= _size)
			return;

		size = min(size, _size - offset);

		if (_capacity > 0)
		{
			_size -= size;
			::memmove(const_cast<char*>(_text) + offset, _text + offset + size, _size - offset);
			const_cast<char*>(_text)[_size] = '\0';
			return;
		}

		const auto old_text = _text;
		Private::initialize(*this, size + 1);

		if (offset)
			::memcpy(const_cast<char*>(_text), old_text, offset);

		_size -= size;
		::memcpy(const_cast<char*>(_text) + offset, old_text + offset + size, _size - offset);
		const_cast<char*>(_text)[_size] = '\0';
	}

	void String::reserve(size_t size)
	{
		size_t capacity = size + 1;
		if (_capacity > 0)
		{
			Private::grow(*this, capacity);
		}
		else
		{
			const auto old_text = _text;
			Private::initialize(*this, max(_size + 1, capacity));
			::memcpy(const_cast<char*>(_text), old_text, _size + 1);
			const_cast<char*>(_text)[_size] = '\0';
		}
	}

	void String::resize(size_t size)
	{
		const size_t capacity = size + 1;
		if (_capacity > 0)
		{
			Private::grow(*this, capacity);
		}
		else
		{
			const auto old_text = _text;
			Private::initialize(*this, max(_size + 1, capacity));
			::memcpy(const_cast<char*>(_text), old_text, _size + 1);
		}

		const_cast<char*>(_text)[size] = '\0';
		_size = size;
	}

	void String::truncate(size_t size)
	{
		if (size >= _size)
			return;

		if (_capacity > 0)
		{
			const_cast<char*>(_text)[size] = '\0';
			_size = size;
			return;
		}

		const auto old_text = _text;
		Private::initialize(*this, size + 1);
		::memcpy(const_cast<char*>(_text), old_text, size);
		const_cast<char*>(_text)[size] = '\0';
		_size = size;
	}

	String& String::swap(String* string) noexcept
	{
		const auto text = _text;
		const auto size = _size;
		const auto capacity = _capacity;
		const auto allocator = _allocator;

		_text = string->_text;
		_size = string->_size;
		_capacity = string->_capacity;
		_allocator = string->_allocator;

		string->_text = text;
		string->_size = size;
		string->_capacity = capacity;
		string->_allocator = allocator;

		return *this;
	}

	String& String::swap(String&& string) noexcept
	{
		const auto text = _text;
		const auto size = _size;
		const auto capacity = _capacity;
		const auto allocator = _allocator;

		_text = string._text;
		_size = string._size;
		_capacity = string._capacity;
		_allocator = string._allocator;

		string._text = text;
		string._size = size;
		string._capacity = capacity;
		string._allocator = allocator;

		return *this;
	}

	String& String::trim()
	{
		const StaticString& trimmed_string = this->trimmed();
		if (trimmed_string.size() < _size)
		{
			const size_t capacity = trimmed_string.size() + 1;
			if (_capacity > 0)
				Private::grow(*this, capacity);
			else
				Private::initialize(*this, capacity);

			::memmove(const_cast<char*>(_text), trimmed_string.text(), trimmed_string.size());
			const_cast<char*>(_text)[trimmed_string.size()] = '\0';
			_size = trimmed_string.size();
		}
		return *this;
	}

	String& String::operator=(const StaticString& string)
	{
		const auto capacity = string.size() + 1;
		if (_capacity > 0)
			Private::grow(*this, capacity);
		else
			Private::initialize(*this, capacity);

		::memcpy(const_cast<char*>(_text), string.text(), string.size());
		const_cast<char*>(_text)[string.size()] = '\0';
		_size = string.size();

		return *this;
	}

	String& String::operator=(String&& string) noexcept
	{
		if (_capacity > 0)
			_allocator->deallocate(const_cast<char*>(_text));
		_text = string._text;
		_size = string._size;
		_capacity = string._capacity;
		_allocator = string._allocator;
		string._capacity = 0;
		return *this;
	}
}
