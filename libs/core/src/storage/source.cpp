//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/storage/source.h>
#include "source.h"

#include <yttrium/memory/buffer.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <system_error>

namespace Yt
{
	class MemorySource final : public Source
	{
	public:
		MemorySource(const void* data, size_t size) noexcept
			: _data{ data }, _size{ size } {}

		const void* data() const noexcept override
		{
			return _data;
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const noexcept override
		{
			if (offset >= _size)
				return 0;
			const auto actual_size = static_cast<size_t>(std::min<uint64_t>(size, _size - offset));
			std::memcpy(data, static_cast<const uint8_t*>(_data) + offset, actual_size);
			return actual_size;
		}

		uint64_t size() const noexcept override
		{
			return _size;
		}

	private:
		const void* const _data;
		const size_t _size;
	};

	class BufferSource final : public Source
	{
	public:
		explicit BufferSource(Buffer&& buffer)
			: _buffer{ std::make_shared<const Buffer>(std::move(buffer)) } {}
		explicit BufferSource(const std::shared_ptr<const Buffer>& buffer) noexcept
			: _buffer{ buffer } {}

		const void* data() const noexcept override
		{
			return _buffer->data();
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const noexcept override
		{
			if (offset >= _buffer->size())
				return 0;
			const auto actual_size = static_cast<size_t>(std::min<uint64_t>(size, _buffer->size() - offset));
			std::memcpy(data, static_cast<const uint8_t*>(_buffer->data()) + offset, actual_size);
			return actual_size;
		}

		uint64_t size() const noexcept override
		{
			return _buffer->size();
		}

	private:
		const std::shared_ptr<const Buffer> _buffer;
	};

	class ProxySource final : public Source
	{
	public:
		ProxySource(const std::shared_ptr<const Source>& source, uint64_t base, uint64_t size) noexcept
			: _source{ source }, _base{ base }, _size{ size } {}

		size_t read_at(uint64_t offset, void* data, size_t size) const noexcept override
		{
			return offset <= std::numeric_limits<uint64_t>::max() - _base ? _source->read_at(_base + offset, data, static_cast<size_t>(std::min<uint64_t>(size, _size - offset))) : 0;
		}

		uint64_t size() const noexcept override
		{
			return _size;
		}

	private:
		const std::shared_ptr<const Source> _source;
		const uint64_t _base;
		const uint64_t _size;
	};

	std::unique_ptr<Source> Source::from(const void* data, size_t size)
	{
		return std::make_unique<MemorySource>(data, size);
	}

	std::unique_ptr<Source> Source::from(Buffer&& buffer)
	{
		return std::make_unique<BufferSource>(std::move(buffer));
	}

	std::unique_ptr<Source> create_source(const std::shared_ptr<const Buffer>& buffer)
	{
		return std::make_unique<BufferSource>(buffer);
	}

	std::unique_ptr<Source> Source::from(const std::shared_ptr<const Source>& source, uint64_t base, uint64_t size)
	{
		const auto source_size = source->size();
		const auto actual_base = std::min(base, source_size);
		return std::make_unique<ProxySource>(source, actual_base, std::min(size, source_size - actual_base));
	}

	Buffer Source::to_buffer(size_t padding_size) const
	{
		const auto source_size = size();
		if (source_size > std::numeric_limits<size_t>::max() - padding_size)
			throw std::bad_alloc{};
		const auto size = static_cast<size_t>(source_size);
		Buffer buffer{ size + padding_size };
		if (read_at(0, buffer.data(), size) != size)
			throw std::system_error{ std::make_error_code(std::errc::io_error) };
		std::memset(buffer.begin() + size, 0, padding_size);
		buffer.resize(size);
		return buffer;
	}

	std::string Source::to_string() const
	{
		const auto source_size = size();
		if (source_size >= std::numeric_limits<size_t>::max()) // One extra byte for null terminator.
			throw std::bad_alloc{};
		std::string string(static_cast<size_t>(source_size), '\0');
		if (read_at(0, string.data(), string.size()) != string.size())
			throw std::system_error{ std::make_error_code(std::errc::io_error) };
		return string;
	}
}
