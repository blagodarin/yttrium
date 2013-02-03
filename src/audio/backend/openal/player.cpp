#include "player.h"

#include <Yttrium/assert.h>

namespace Yttrium
{

OpenAlPlayer::OpenAlPlayer()
{
	alGenBuffers(NumBuffers, _buffers);
	if (alGetError() == AL_NO_ERROR)
	{
		alGenSources(1, &_source);
		if (alGetError() == AL_NO_ERROR)
		{
			alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
			return;
		}
		alDeleteBuffers(NumBuffers, _buffers);
	}

	Y_ABORT("Couldn't create OpenAlPlayer"); // NOTE: Safe to continue.

	_source = 0;
	for (size_t i = 0; i < NumBuffers; ++i)
	{
		_buffers[i] = 0;
	}
}

OpenAlPlayer::~OpenAlPlayer()
{
	alDeleteSources(1, &_source);
	alDeleteBuffers(NumBuffers, _buffers);
}

bool OpenAlPlayer::set_format(const AudioFormat &format)
{
	switch (format.depth)
	{
	case 1: // 8 bits per sample.

		switch (format.channels)
		{
		case 1:  _format = AL_FORMAT_MONO8;   break;
		case 2:  _format = AL_FORMAT_STEREO8; break;
		default: return false;
		}
		break;

	case 2: // 16 bits per sample.

		switch (format.channels)
		{
		case 1:  _format = AL_FORMAT_MONO16;   break;
		case 2:  _format = AL_FORMAT_STEREO16; break;
		default: return false;
		}
		break;

	default:

		return false;
	}
	_frequency = format.frequency; // TODO: Check the frequency too.
	return true;
}

void OpenAlPlayer::fill_buffer(size_t index, void *data, size_t size)
{
	alBufferData(_buffers[index], _format, data, size, _frequency);
	alSourceQueueBuffers(_source, 1, &_buffers[index]);
}

size_t OpenAlPlayer::check_buffers()
{
	ALint processed = 0;
	alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
	return processed;
}

void OpenAlPlayer::refill_buffer(void *data, size_t size)
{
	ALuint buffer;

	alSourceUnqueueBuffers(_source, 1, &buffer);
	alBufferData(buffer, _format, data, size, _frequency);
	alSourceQueueBuffers(_source, 1, &buffer);
}

void OpenAlPlayer::play()
{
	alSourcePlay(_source);
}

void OpenAlPlayer::pause()
{
	alSourcePause(_source);
}

void OpenAlPlayer::stop()
{
	ALint  processed = 0; // Suppressing Valgrind warning.
	ALuint buffers[NumBuffers];

	alSourceStop(_source);
	alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
	alSourceUnqueueBuffers(_source, processed, buffers);
}

} // namespace Yttrium
