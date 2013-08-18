#ifndef __AUDIO_BACKEND_PLAYER_H
#define __AUDIO_BACKEND_PLAYER_H

#include <yttrium/audio/player.h>

namespace Yttrium
{

class AudioPlayerBackend
{
public:

	enum
	{
		NumBuffers = 2,
	};

public:

	virtual ~AudioPlayerBackend()
	{
	}

public:

	virtual bool set_format(const AudioFormat &format) = 0;

	virtual void fill_buffer(size_t index, void *data, size_t size) = 0;

	virtual size_t check_buffers() = 0;

	virtual void refill_buffer(void *data, size_t size) = 0;

	virtual void play() = 0;

	virtual void pause() = 0;

	virtual void stop() = 0;

public:

	static AudioPlayerBackend *create(Allocator *allocator);
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_PLAYER_H
