#include "streamer.h"

#include "backend/player.h"

namespace Yttrium
{

enum
{
	BufferSize = 1000, // 1 s buffer.
};

void AudioStreamer::close()
{
	_source.close();
}

AudioStreamer::FetchResult AudioStreamer::fetch()
{
	size_t size = read();

	if (!size)
	{
		return NoMoreData;
	}

	_backend->refill_buffer(_buffer.data(), size);

	return (size < _buffer.size())
		? NotEnoughData
		: Ok;
}

bool AudioStreamer::open(const StaticString &name, const AudioPlayer::Settings &settings, AudioType type, AudioPlayer::Order order)
{
	if (_source.open(name, type, _allocator))
	{
		AudioFormat format = _source.format();

		_buffer_length = format.frequency * BufferSize / 1000;

		// NOTE: Currently, music audio must be at least <NumBuffers> buffers long.
		// I have no idea whether this is a real bug.

		UOffset source_size = _source.size();

		if (source_size >= _buffer_length * AudioPlayerBackend::NumBuffers)
		{
			// NOTE: If loop position is past the end or within 1 buffer before it, no looping would be performed.

			_begin_sample = static_cast<UOffset>(settings.begin * format.frequency);
			_end_sample   = static_cast<UOffset>(settings.end   * format.frequency);

			if (!_end_sample || _end_sample > source_size)
			{
				_end_sample = source_size;
			}

			if (_begin_sample < _end_sample && _end_sample - _begin_sample >= _buffer_length)
			{
				if (order == AudioPlayer::Random)
				{
					_loop_sample = static_cast<UOffset>(settings.loop * format.frequency);
					_is_looping = (_loop_sample < _end_sample && _end_sample - _loop_sample >= _buffer_length);
				}
				else
				{
					_is_looping = false;
				}

				if (_backend->set_format(format))
				{
					_atom = format.atom_size();
					_buffer.resize(_buffer_length * _atom);
					_source.seek(_begin_sample);
					return true;
				}
			}
		}

		_source.close();
	}

	return false;
}

void AudioStreamer::prefetch()
{
	for (size_t i = 0; i < AudioPlayerBackend::NumBuffers; ++i)
	{
		_backend->fill_buffer(i, _buffer.data(), read());
	}
}

size_t AudioStreamer::read()
{
	size_t size;

	if (_end_sample - _source.offset() >= _buffer_length)
	{
		size = _source.read(_buffer.data(), _buffer.size());
	}
	else
	{
		size_t tail = (_end_sample - _source.offset()) * _atom;
		size = _source.read(_buffer.data(), tail);
		if (_is_looping)
		{
			_source.seek(_loop_sample);
			size += _source.read(_buffer.data(size), _buffer.size() - tail);
		}
	}

	return size;
}

} // namespace Yttrium
