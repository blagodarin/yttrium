#include "streamer.h"

#include "backend.h"

namespace Yttrium
{
	enum
	{
		BufferSize = 1000, // 1 s buffer.
	};

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

	bool AudioStreamer::open(const StaticString& name, const AudioPlayer::Settings& settings, AudioType type, AudioPlayer::Order order)
	{
		_source = AudioReader::open(name, type, &_allocator);
		if (!_source)
			return false;

		const AudioFormat& format = _source->format();

		_buffer_units = format.frequency * BufferSize / 1000;

		// NOTE: Currently, music audio must be at least <NumBuffers> buffers long.
		// I have no idea whether this is a real bug.

		const uint64_t source_units = _source->size() / format.unit_size();

		if (source_units >= _buffer_units * AudioPlayerBackend::NumBuffers)
		{
			// NOTE: If loop position is past the end or within 1 buffer before it, no looping would be performed.

			_begin_sample = static_cast<uint64_t>(settings.begin * format.frequency);
			_end_sample = static_cast<uint64_t>(settings.end * format.frequency);

			if (_end_sample == 0 || _end_sample > source_units)
				_end_sample = source_units;

			if (_begin_sample < _end_sample && _end_sample - _begin_sample >= _buffer_units)
			{
				if (order == AudioPlayer::Random)
				{
					_loop_sample = static_cast<uint64_t>(settings.loop * format.frequency);
					_is_looping = (_loop_sample < _end_sample && _end_sample - _loop_sample >= _buffer_units);
				}
				else
				{
					_is_looping = false;
				}

				if (_backend.set_format(format))
				{
					_unit_size = format.unit_size();
					_buffer.resize(_buffer_units * _unit_size);
					_source->seek(_begin_sample);
					return true;
				}
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
		size_t size = _source->read(_buffer.data(), tail);
		if (_is_looping)
		{
			_source->seek(_loop_sample);
			size += _source->read(_buffer.data(size), _buffer.size() - tail);
		}
		return size;
	}
}
