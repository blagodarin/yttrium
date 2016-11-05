#include "streamer.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include "backend.h"

namespace Yttrium
{
	AudioStreamer::AudioStreamer(AudioPlayerBackend& backend, Allocator& allocator)
		: _allocator(allocator)
		, _backend(backend)
	{
	}

	AudioStreamer::~AudioStreamer() = default;

	void AudioStreamer::close()
	{
		_source = {};
	}

	AudioStreamer::FetchResult AudioStreamer::fetch()
	{
		const size_t size = read();
		if (!size)
			return NoMoreData;
		_backend.refill_buffer(_buffer.data(), size);
		return size < _buffer.size() ? NotEnoughData : Ok;
	}

	bool AudioStreamer::open(Reader&& reader, const AudioPlayer::Settings& settings, AudioPlayer::Order order)
	{
		_source = AudioReader::open(std::move(reader));
		if (!_source)
			return false;

		const auto format = _source->format();

		_buffer_units = format.samples_per_second(); // One-second buffer.

		// NOTE: Currently, music audio must be at least <NumBuffers> buffers long.
		// I have no idea whether this is a real bug.

		const uint64_t source_units = _source->size() / format.unit_size();

		if (source_units >= _buffer_units * AudioPlayerBackend::NumBuffers)
		{
			// NOTE: If loop position is past the end or within 1 buffer before it, no looping would be performed.

			_begin_sample = static_cast<uint64_t>(settings.begin * format.samples_per_second());
			_end_sample = static_cast<uint64_t>(settings.end * format.samples_per_second());

			if (_end_sample == 0 || _end_sample > source_units)
				_end_sample = source_units;

			if (_begin_sample < _end_sample && _end_sample - _begin_sample >= _buffer_units)
			{
				if (order == AudioPlayer::Random)
				{
					_loop_sample = static_cast<uint64_t>(settings.loop * format.samples_per_second());
					_is_looping = (_loop_sample < _end_sample && _end_sample - _loop_sample >= _buffer_units);
				}
				else
				{
					_is_looping = false;
				}
				_backend.set_format(format);
				_unit_size = format.unit_size();
				_buffer.reset(_buffer_units * _unit_size);
				_source->seek(_begin_sample);
				return true;
			}
		}

		_source = {};
		return false;
	}

	void AudioStreamer::prefetch()
	{
		for (size_t i = 0; i < AudioPlayerBackend::NumBuffers; ++i)
			_backend.fill_buffer(i, _buffer.data(), read());
	}

	size_t AudioStreamer::read()
	{
		if (_end_sample - _source->offset() >= _buffer_units)
			return _source->read(_buffer.data(), _buffer.size());

		const size_t tail = (_end_sample - _source->offset()) * _unit_size;
		auto size = _source->read(_buffer.data(), tail);
		if (_is_looping)
		{
			_source->seek(_loop_sample);
			size += _source->read(&_buffer[size], _buffer.size() - tail);
		}
		return size;
	}
}
