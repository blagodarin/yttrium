//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/utils/tiny_string_map.h>

#include <cstring>

static_assert(sizeof(uint8_t) == sizeof(char));

namespace Yttrium
{
	TinyStringMap::TinyStringMap(const TinyStringMap& other)
		: _data{ std::make_unique<uint8_t[]>(other._size) }
		, _capacity{ other._size }
		, _size{ other._size }
	{
		std::memcpy(_data.get(), other._data.get(), _size);
	}

	TinyStringMap::TinyStringMap(TinyStringMap&& other) noexcept
		: _data{ std::move(other._data) }
		, _capacity{ other._capacity }
		, _size{ other._size }
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

	void TinyStringMap::erase(std::string_view key) noexcept
	{
		const auto value = find(key);
		if (!value.data())
			return;
		const auto node_begin = value.data() - 1 - key.size() - 1;
		const auto node_end = value.data() + *reinterpret_cast<const uint8_t*>(value.data() - 1);
		std::memmove(const_cast<char*>(node_begin), node_end, static_cast<size_t>(reinterpret_cast<const char*>(_data.get() + _size) - node_end));
		_size -= static_cast<size_t>(node_end - node_begin);
	}

	std::string_view TinyStringMap::find(std::string_view key) const noexcept
	{
		for (auto current = _data.get(); current < _data.get() + _size;)
		{
			const auto current_key_size = *current++;
			if (current_key_size == key.size() && !std::memcmp(current, key.data(), current_key_size))
			{
				const auto value = current + current_key_size;
				return { reinterpret_cast<const char*>(value + 1), *value };
			}
			current += current_key_size;
			current += 1 + *current;
		}
		return {};
	}

	void TinyStringMap::insert_or_assign(std::string_view key, std::string_view value)
	{
		const auto key_size = static_cast<uint8_t>(key.size());
		const auto value_size = static_cast<uint8_t>(value.size());
		if (key_size != key.size() || value_size != value.size())
			throw std::length_error("Tiny string size limit exceeded");
		erase(key);
		reserve_bytes(_size + 1 + key_size + 1 + value_size);
		auto dst = _data.get() + _size;
		*dst++ = key_size;
		std::memcpy(dst, key.data(), key_size);
		dst += key_size;
		*dst++ = value_size;
		std::memcpy(dst, value.data(), value_size);
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
