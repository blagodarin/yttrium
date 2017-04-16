#include <yttrium/tiny_string_map.h>

#include <yttrium/static_string.h>

#include <cstring>

static_assert(sizeof(uint8_t) == sizeof(char), "Unexpected char size");

namespace Yttrium
{
	TinyStringMap::TinyStringMap(const TinyStringMap& other)
		: _data(std::make_unique<uint8_t[]>(other._size))
		, _capacity(other._size)
		, _size(other._size)
	{
		std::memcpy(_data.get(), other._data.get(), _size);
	}

	TinyStringMap::TinyStringMap(TinyStringMap&& other) noexcept
		: _data(std::move(other._data))
		, _capacity(other._capacity)
		, _size(other._size)
	{
		other._capacity = 0;
		other._size = 0;
	}

	TinyStringMap& TinyStringMap::operator=(const TinyStringMap& other)
	{
		if (_capacity < other._capacity)
		{
			_data = std::make_unique<uint8_t[]>(other._size);
			_capacity = other._size;
		}
		std::memcpy(_data.get(), other._data.get(), other._size);
		_size = other._size;
		return *this;
	}

	TinyStringMap& TinyStringMap::operator=(TinyStringMap&& other) noexcept
	{
		_data = std::move(other._data);
		_capacity = other._capacity;
		_size = other._size;
		other._capacity = 0;
		other._size = 0;
		return *this;
	}

	void TinyStringMap::erase(const StaticString& key) noexcept
	{
		const auto value = find(key);
		if (value.is_null())
			return;
		const auto node_begin = value.text() - 1 - key.size() - 1;
		const auto node_end = value.text() + *reinterpret_cast<const uint8_t*>(value.text() - 1);
		std::memmove(const_cast<char*>(node_begin), node_end, static_cast<size_t>(reinterpret_cast<const char*>(_data.get() + _size) - node_end));
		_size -= static_cast<size_t>(node_end - node_begin);
	}

	StaticString TinyStringMap::find(const StaticString& key) const noexcept
	{
		for (auto current = _data.get(); current < _data.get() + _size; )
		{
			const auto current_key_size = *current++;
			if (current_key_size == key.size() && !std::memcmp(current, key.text(), current_key_size))
			{
				const auto value = current + current_key_size;
				return { reinterpret_cast<const char*>(value + 1), *value };
			}
			current += current_key_size;
			current += 1 + *current;
		}
		return {};
	}

	void TinyStringMap::insert_or_assign(const StaticString& key, const StaticString& value)
	{
		const auto key_size = static_cast<uint8_t>(key.size());
		const auto value_size = static_cast<uint8_t>(value.size());
		if (key_size != key.size() || value_size != value.size())
			throw std::length_error("Tiny string size limit exceeded");
		erase(key);
		reserve_bytes(_size + 1 + key_size + 1 + value_size);
		auto dst = _data.get() + _size;
		*dst++ = key_size;
		std::memcpy(dst, key.text(), key_size);
		dst += key_size;
		*dst++ = value_size;
		std::memcpy(dst, value.text(), value_size);
		dst += value_size;
		_size = static_cast<size_t>(dst - _data.get());
	}

	void TinyStringMap::reserve_bytes(size_t required_capacity)
	{
		if (required_capacity <= _capacity)
			return;
		auto new_data = std::make_unique<uint8_t[]>(required_capacity);
		std::memcpy(new_data.get(), _data.get(), _size);
		_data = std::move(new_data);
		_capacity = required_capacity;
	}
}
