#include <yttrium/string.h>

#include <yttrium/memory/allocator.h>
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
			const auto new_capacity = next_power_of_2(capacity);
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
			const auto actual_capacity = next_power_of_2(max(capacity, MinCapacity));
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
		, _capacity(next_power_of_2(max(_size + 1, Private::MinCapacity)))
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
		, _capacity(next_power_of_2(max(_size + 1, Private::MinCapacity)))
		, _allocator(allocator)
	{
		Private::initialize_copy(*this, string);
	}

	String::~String()
	{
		if (_capacity > 0)
			_allocator->deallocate(const_cast<char*>(_text));
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
