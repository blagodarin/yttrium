#ifndef __AUDIO_STREAMER_H
#define __AUDIO_STREAMER_H

#include <yttrium/audio/io.h>
#include <yttrium/audio/player.h>
#include <yttrium/buffer.h>

namespace Yttrium
{

class AudioPlayerBackend;

class AudioStreamer
{
public:

	enum FetchResult
	{
		Ok,
		NotEnoughData,
		NoMoreData,
	};

public:

	AudioStreamer(AudioPlayerBackend* backend, Allocator* allocator)
		: _allocator(allocator)
		, _backend(backend)
		, _buffer(_allocator)
	{
	}

	void close();

	FetchResult fetch();

	bool open(const StaticString& name, const AudioPlayer::Settings& settings, AudioType type, AudioPlayer::Order order);

	void prefetch();

private:

	size_t read();

private:

	Allocator*          _allocator;
	AudioPlayerBackend* _backend;
	AudioReaderPtr      _source;
	uint64_t            _begin_sample;
	uint64_t            _end_sample;
	bool                _is_looping;
	uint64_t            _loop_sample;
	Buffer              _buffer;
	size_t              _unit_size;
	size_t              _buffer_units;
};

} // namespace Yttrium

#endif // __AUDIO_STREAMER_H
