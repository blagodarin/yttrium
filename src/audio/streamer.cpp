#include "streamer.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include "backend.h"
#include "music.h"

namespace Yttrium
{
	AudioStreamer::AudioStreamer(AudioPlayerBackend& backend)
		: _backend(backend)
	{
	}

	AudioStreamer::~AudioStreamer()
	{
		_backend.stop();
	}

	AudioStreamer::FetchResult AudioStreamer::fetch()
	{
		const size_t size = read();
		if (!size)
			return NoMoreData;
		_backend.refill_buffer(_buffer.data(), size);
		return size < _buffer.size() ? NotEnoughData : Ok;
	}

	bool AudioStreamer::open(const ResourcePtr<const Music>& music)
	{
		_music = music;
		if (!_music)
			return false;

		auto& reader = static_cast<const MusicImpl&>(*_music).reader();
		const auto settings = static_cast<const MusicImpl&>(*_music).settings();

		const auto format = reader.format();

		_buffer_samples = format.samples_per_second(); // One-second buffer.

		// NOTE: Currently, music audio must be at least <NumBuffers> buffers long.

		const uint64_t total_samples = reader.total_bytes() / format.block_size();

		if (total_samples >= _buffer_samples * AudioPlayerBackend::NumBuffers)
		{
			// NOTE: If loop position is past the end or within 1 buffer before it, no looping would be performed.

			_start_sample = static_cast<uint64_t>(settings.start * format.samples_per_second());
			_end_sample = static_cast<uint64_t>(settings.end * format.samples_per_second());

			if (_end_sample == 0 || _end_sample > total_samples)
				_end_sample = total_samples;

			if (_start_sample < _end_sample && _end_sample - _start_sample >= _buffer_samples)
			{
				_loop_sample = static_cast<uint64_t>(settings.loop * format.samples_per_second());
				_is_looping = (_loop_sample < _end_sample && _end_sample - _loop_sample >= _buffer_samples);
				_backend.set_format(format);
				_block_size = format.block_size();
				_buffer.reset(_buffer_samples * _block_size);
				reader.seek(_start_sample);
				return true;
			}
		}

		_music = {};
		return false;
	}

	void AudioStreamer::start()
	{
		for (size_t i = 0; i < AudioPlayerBackend::NumBuffers; ++i)
			_backend.fill_buffer(i, _buffer.data(), read());
		_backend.play();
	}

	size_t AudioStreamer::read()
	{
		auto& reader = static_cast<const MusicImpl&>(*_music).reader();

		if (_end_sample - reader.current_sample() >= _buffer_samples)
			return reader.read(_buffer.data(), _buffer.size());

		const size_t tail = (_end_sample - reader.current_sample()) * _block_size;
		auto size = reader.read(_buffer.data(), tail);
		if (_is_looping)
		{
			reader.seek(_loop_sample);
			size += reader.read(&_buffer[size], _buffer.size() - tail);
		}
		return size;
	}
}
