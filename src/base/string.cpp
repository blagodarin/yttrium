#include <yttrium/string.h>

#include <yttrium/utils.h>

#include <cassert>
#include <cstring>

namespace Yttrium
{
	struct String::Private
	{
		static const size_t MinCapacity = 16;

		static void grow(String& s, size_t capacity)
		{
			assert(s._capacity > 0);
			if (capacity <= s._capacity)
				return;
			const auto new_capacity = next_power_of_2(capacity);
			const auto new_text = new char[new_capacity];
			std::memcpy(new_text, s._text, s._size); // Don't copy the '\0', it will be written to the string later.
			delete[] s._text;
			s._text = new_text;
			s._capacity = new_capacity;
		}

		static void initialize(String& s, size_t capacity)
		{
			assert(s._capacity == 0);
			const auto actual_capacity = next_power_of_2(std::max(capacity, MinCapacity));
			s._text = new char[actual_capacity];
			s._capacity = actual_capacity;
		}

		static void initialize_copy(String& s, const StaticString& source)
		{
			assert(!s._text);
			assert(s._size == source.size());
			assert(s._capacity > 0);
			const auto text = new char[s._capacity];
			std::memcpy(text, source.text(), source.size());
			text[source.size()] = '\0';
			s._text = text;
		}
	};

	const size_t String::Private::MinCapacity;

	String::String(const String& string)
		: StaticString(nullptr, string._size)
		, _capacity(next_power_of_2(std::max(_size + 1, Private::MinCapacity)))
	{
		Private::initialize_copy(*this, string);
	}

	String::String(String&& string) noexcept
		: StaticString(string._text, string._size)
		, _capacity(string._capacity)
	{
		string._capacity = 0;
	}

	String::String(const StaticString& string)
		: StaticString(nullptr, string.size())
		, _capacity(next_power_of_2(std::max(_size + 1, Private::MinCapacity)))
	{
		Private::initialize_copy(*this, string);
	}

	String::~String()
	{
		if (_capacity > 0)
			delete[] _text;
	}

	String& String::operator=(const StaticString& string)
	{
		const auto capacity = string.size() + 1;
		if (_capacity > 0)
			Private::grow(*this, capacity);
		else
			Private::initialize(*this, capacity);

		std::memcpy(const_cast<char*>(_text), string.text(), string.size());
		const_cast<char*>(_text)[string.size()] = '\0';
		_size = string.size();

		return *this;
	}

	String& String::operator=(String&& string) noexcept
	{
		if (_capacity > 0)
			delete[] _text;
		_text = string._text;
		_size = string._size;
		_capacity = string._capacity;
		string._capacity = 0;
		return *this;
	}
}
