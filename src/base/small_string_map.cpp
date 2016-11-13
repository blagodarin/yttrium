#include <yttrium/small_string_map.h>

#include <cstring>

// TODO: Benchmarks.

static_assert(sizeof(uint8_t) == sizeof(char), "Unexpected char size");

namespace Yttrium
{
	SmallStringMap::SmallStringMap(const SmallStringMap& other)
		: _data(std::make_unique<uint8_t[]>(other._size))
		, _size(other._size)
		, _capacity(other._size)
	{
		std::memcpy(_data.get(), other._data.get(), _size);
	}

	void SmallStringMap::erase(const StaticString& key) noexcept
	{
		const auto value = find(key);
		if (value.is_null())
			return;
		const auto node_begin = value.text() - key.size() - 3;
		const auto node_end = value.text() + *reinterpret_cast<const uint8_t*>(value.text() - 1) + 1;
		std::memmove(const_cast<char*>(node_begin), node_end, reinterpret_cast<const char*>(_data.get() + _size) - node_end);
		_size -= node_end - node_begin;
	}

	StaticString SmallStringMap::find(const StaticString& key) const noexcept
	{
		for (auto current = _data.get(); current < _data.get() + _size; )
		{
			const auto current_key_size = *current++;
			if (current_key_size == key.size() && !std::memcmp(current, key.text(), current_key_size))
			{
				const auto value = current + current_key_size + 1;
				return { reinterpret_cast<const char*>(value + 1), *value };
			}
			current += current_key_size + 1;
			current += *current + 2;
		}
		return {};
	}

	void SmallStringMap::insert_or_assign(const StaticString& key, const StaticString& value)
	{
		const auto key_size = static_cast<uint8_t>(key.size());
		const auto value_size = static_cast<uint8_t>(value.size());
		if (key_size != key.size() || value_size != value.size())
			throw std::length_error("Small string size must not exceed 255");

		erase(key);

		const auto required_capacity = _size + key_size + value_size + 4;
		if (required_capacity > _capacity)
		{
			auto new_data = std::make_unique<uint8_t[]>(required_capacity);
			std::memcpy(new_data.get(), _data.get(), _size);
			_data = std::move(new_data);
			_capacity = required_capacity;
		}

		auto dst = _data.get() + _size;
		*dst++ = key_size;
		std::memcpy(dst, key.text(), key_size);
		dst += key_size;
		*dst++ = '\0';
		*dst++ = value_size;
		std::memcpy(dst, value.text(), value_size);
		dst += value_size;
		*dst++ = '\0';
		_size = dst - _data.get();
	}
}
