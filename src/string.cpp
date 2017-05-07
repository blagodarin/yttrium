#include <yttrium/string.h>

#include <yttrium/utils.h>

#include <algorithm>
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
			std::memcpy(new_text, s._data, s._size); // Don't copy the '\0', it will be written to the string later.
			delete[] s._data;
			s._data = new_text;
			s._capacity = new_capacity;
		}

		static void initialize(String& s, size_t capacity)
		{
			assert(s._capacity == 0);
			const auto actual_capacity = next_power_of_2(std::max(capacity, MinCapacity));
			s._data = new char[actual_capacity];
			s._capacity = actual_capacity;
		}

		static void initialize_copy(String& s, std::string_view source)
		{
			assert(!s._data);
			assert(s._size == source.size());
			assert(s._capacity > 0);
			const auto data = new char[s._capacity];
			std::memcpy(data, source.data(), source.size());
			data[source.size()] = '\0';
			s._data = data;
		}
	};

	const size_t String::Private::MinCapacity;

	String::String(const String& string)
		: _data{nullptr}
		, _size{string._size}
		, _capacity{next_power_of_2(std::max(_size + 1, Private::MinCapacity))}
	{
		Private::initialize_copy(*this, string);
	}

	String::String(String&& string) noexcept
		: _data{string._data}
		, _size{string._size}
		, _capacity{string._capacity}
	{
		string._capacity = 0;
	}

	String::String(std::string_view string)
		: _data{nullptr}
		, _size{string.size()}
		, _capacity{next_power_of_2(std::max(_size + 1, Private::MinCapacity))}
	{
		Private::initialize_copy(*this, string);
	}

	String::~String()
	{
		if (_capacity > 0)
			delete[] _data;
	}

	String& String::operator=(std::string_view string)
	{
		const auto capacity = string.size() + 1;
		if (_capacity > 0)
			Private::grow(*this, capacity);
		else
			Private::initialize(*this, capacity);

		std::memcpy(const_cast<char*>(_data), string.data(), string.size());
		const_cast<char*>(_data)[string.size()] = '\0';
		_size = string.size();

		return *this;
	}

	String& String::operator=(String&& string) noexcept
	{
		if (_capacity > 0)
			delete[] _data;
		_data = string._data;
		_size = string._size;
		_capacity = string._capacity;
		string._capacity = 0;
		return *this;
	}

	char String::Null = '\0';
}
