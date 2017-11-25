#include "music.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include <yttrium/utils.h>
#include "backend.h"

#include <cassert>

namespace Yttrium
{
	MusicReaderImpl::MusicReaderImpl(std::unique_ptr<AudioReader>&& reader)
		: _reader{std::move(reader)}
		, _block_size{_reader->format().block_size()}
		, _buffer_samples{_reader->format().samples_per_second()} // One-second buffers.
		, _end_sample{_reader->total_samples()}
	{
		assert(_end_sample - _start_sample >= AudioPlayerBackend::NumBuffers * _buffer_samples); // TODO: Harder check.
	}

	bool MusicReaderImpl::set_properties(int start_ms, int end_ms, int loop_ms) noexcept
	{
		if (start_ms < 0 || !(end_ms == 0 || end_ms >= start_ms) || !(loop_ms >= 0 && loop_ms <= end_ms))
			return false;

		const auto total_samples = _reader->total_samples();
		const auto samples_per_second = _reader->format().samples_per_second();

		const auto start_sample = to_unsigned(start_ms) * samples_per_second / 1000;
		if (start_sample > total_samples)
			return false;

		const auto end_sample = end_ms > 0 ? std::min(to_unsigned(end_ms) * samples_per_second / 1000, total_samples) : total_samples;
		if (end_sample - start_sample < AudioPlayerBackend::NumBuffers * samples_per_second)
			return false;

		_start_sample = start_sample;
		_end_sample = end_sample;
		_loop_sample = std::min(to_unsigned(loop_ms) * samples_per_second / 1000, _end_sample);
		_is_looping = _loop_sample <= _end_sample && _end_sample - _loop_sample >= samples_per_second;
		return true;
	}

	AudioFormat MusicReaderImpl::format() const noexcept
	{
		return _reader->format();
	}

	size_t MusicReaderImpl::read(void* buffer)
	{
		if (_end_sample - _reader->current_sample() >= _buffer_samples)
			return _reader->read(buffer, buffer_size());

		const auto tail = (_end_sample - _reader->current_sample()) * _block_size;
		auto size = _reader->read(buffer, tail);
		if (_is_looping)
		{
			_reader->seek(_loop_sample);
			size += _reader->read(static_cast<std::byte*>(buffer) + size, buffer_size() - tail);
		}
		return size;
	}

	void MusicReaderImpl::seek_start()
	{
		_reader->seek(_start_sample);
	}

	std::unique_ptr<MusicReader> MusicReader::open(std::unique_ptr<Source>&& source)
	{
		return source ? std::make_unique<MusicReaderImpl>(AudioReader::open(std::move(source))) : nullptr;
	}
}
