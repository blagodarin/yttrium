#ifndef __AUDIO_BACKEND_OPENAL_PLAYER_H
#define __AUDIO_BACKEND_OPENAL_PLAYER_H

#include "../player.h"
#include "format.h"
#include "openal.h"

namespace Yttrium
{

class OpenAlPlayer: public AudioPlayerBackend
{
public:

	OpenAlPlayer();
	~OpenAlPlayer() override;

private: // AudioPlayerBackend

	bool set_format(const AudioFormat& format) override;
	void fill_buffer(size_t index, void* data, size_t size) override;
	size_t check_buffers() override;
	void refill_buffer(void* data, size_t size) override;
	void play() override;
	void pause() override;
	void stop() override;

private:

	ALuint       _source;
	ALuint       _buffers[NumBuffers];
	OpenAlFormat _format;
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_OPENAL_PLAYER_H
