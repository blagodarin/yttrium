#ifndef __AUDIO_STREAMER_H
#define __AUDIO_STREAMER_H

#include <Yttrium/audio/io.h>
#include <Yttrium/audio/player.h>
#include <Yttrium/buffer.h>

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

	AudioStreamer(AudioPlayerBackend *backend, Allocator *allocator)
		: _allocator(allocator)
		, _backend(backend)
		, _buffer(_allocator)
	{
	}

	void close();

	FetchResult fetch();

	bool open(const StaticString &name, const AudioPlayer::Settings &settings, AudioType type, AudioPlayer::Order order);

	void prefetch();

private:

	size_t read();

private:

	Allocator          *_allocator;
	AudioPlayerBackend *_backend;
	AudioReader         _source;
	UOffset             _begin_sample;
	UOffset             _end_sample;
	bool                _is_looping;
	UOffset             _loop_sample;
	Buffer              _buffer;
	size_t              _atom;
	size_t              _buffer_length;
	size_t              _buffer_size;
};

} // namespace Yttrium

#endif // __AUDIO_STREAMER_H
