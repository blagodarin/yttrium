#ifndef __AUDIO_BACKEND_OPENAL_PLAYER_HPP
#define __AUDIO_BACKEND_OPENAL_PLAYER_HPP

#include "../player.hpp"

#include "openal.hpp"

namespace Yttrium
{

class OpenAlPlayer: public AudioPlayerBackend
{
public:

	OpenAlPlayer();

	virtual ~OpenAlPlayer();

private: // AudioPlayerBackend

	virtual bool set_format(const AudioFormat &format);

	virtual void fill_buffer(size_t index, void *data, size_t size);

	virtual size_t check_buffers();

	virtual void refill_buffer(void *data, size_t size);

	virtual void play();

	virtual void pause();

	virtual void stop();

private:

	ALuint  _source;
	ALuint  _buffers[NumBuffers];
	ALuint  _format;
	ALsizei _frequency;
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_OPENAL_PLAYER_HPP
