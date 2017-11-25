#include "streamer.h"

#include <yttrium/audio/format.h>
#include <yttrium/audio/reader.h>
#include "backend.h"
#include "music.h"

namespace Yttrium
{
	AudioStreamer::AudioStreamer(AudioPlayerBackend& backend)
		: _backend{backend}
	{
	}

	AudioStreamer::~AudioStreamer()
	{
		_backend.stop();
	}

	AudioStreamer::FetchResult AudioStreamer::fetch()
	{
		const auto size = _music->read(_buffer.data());
		if (!size)
			return NoMoreData;
		_backend.refill_buffer(_buffer.data(), size);
		return size < _buffer.size() ? NotEnoughData : Ok;
	}

	bool AudioStreamer::open(const std::shared_ptr<MusicReader>& music)
	{
		_music = std::static_pointer_cast<MusicReaderImpl>(music);
		if (!_music)
			return false;
		_backend.set_format(_music->format());
		_buffer.reset(_music->buffer_size());
		_music->seek_start();
		return true;
	}

	void AudioStreamer::start()
	{
		for (size_t i = 0; i < AudioPlayerBackend::NumBuffers; ++i)
			_backend.fill_buffer(i, _buffer.data(), _music->read(_buffer.data()));
		_backend.play();
	}
}
