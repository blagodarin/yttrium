#include "source.h"

#include <yttrium/memory/buffer.h>

#include <cstring>
#include <limits>

// TODO: Throw some IO exception if Source::to_xxx() fails.

namespace Yttrium
{
	class MemorySource final : public Source
	{
	public:
		MemorySource(const void* data, size_t size) noexcept : Source{size}, _data{data} {}

		const void* data() const noexcept override
		{
			return _data;
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			if (offset >= _size)
				return 0;
			const auto actual_size = std::min(size, _size - offset);
			std::memcpy(data, static_cast<const uint8_t*>(_data) + offset, actual_size);
			return actual_size;
		}

	private:
		const void* const _data;
	};

	class BufferSource final : public Source
	{
	public:
		explicit BufferSource(Buffer&& buffer) : Source{buffer.size()}, _buffer{std::make_shared<const Buffer>(std::move(buffer))} {}
		BufferSource(const std::shared_ptr<const Buffer>& buffer, const std::string& name) : Source{buffer->size(), name}, _buffer{buffer} {}

		const void* data() const noexcept override
		{
			return _buffer->data();
		}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			if (offset >= _size)
				return 0;
			const auto actual_size = std::min(size, _size - offset);
			std::memcpy(data, static_cast<const uint8_t*>(_buffer->data()) + offset, actual_size);
			return actual_size;
		}

	private:
		const std::shared_ptr<const Buffer> _buffer;
	};

	class ProxySource final : public Source
	{
	public:
		ProxySource(const std::shared_ptr<const Source>& source, uint64_t base, uint64_t size) : Source{size}, _source{source}, _base{base} {}

		size_t read_at(uint64_t offset, void* data, size_t size) const override
		{
			return offset <= std::numeric_limits<uint64_t>::max() - _base ? _source->read_at(_base + offset, data, std::min(size, _size - offset)) : 0;
		}

	private:
		const std::shared_ptr<const Source> _source;
		const uint64_t _base;
	};

	std::unique_ptr<Source> Source::from(const void* data, size_t size)
	{
		return std::make_unique<MemorySource>(data, size);
	}

	std::unique_ptr<Source> Source::from(Buffer&& buffer)
	{
		return std::make_unique<BufferSource>(std::move(buffer));
	}

	std::unique_ptr<Source> create_source(const std::shared_ptr<const Buffer>& buffer, const std::string& name)
	{
		return std::make_unique<BufferSource>(buffer, name);
	}

	std::unique_ptr<Source> Source::from(const std::shared_ptr<const Source>& source, uint64_t base, uint64_t size)
	{
		const auto source_size = source->size();
		const auto actual_base = std::min(base, source_size);
		return std::make_unique<ProxySource>(source, actual_base, std::min(size, source_size - actual_base));
	}

	Buffer Source::to_buffer() const
	{
		if (_size >= std::numeric_limits<size_t>::max()) // One extra byte for null terminator.
			throw std::bad_alloc();
		Buffer buffer{_size + 1};
		if (read_at(0, buffer.data(), _size) != _size)
			return {};
		buffer[_size] = '\0';
		buffer.resize(_size);
		return buffer;
	}

	std::string Source::to_string() const
	{
		if (_size >= std::numeric_limits<size_t>::max()) // One extra byte for null terminator.
			throw std::bad_alloc();
		std::string string(_size, '\0');
		if (read_at(0, const_cast<char*>(string.data()), string.size()) != string.size()) // TODO-17: Remove const_cast.
			return {};
		return string;
	}
}
