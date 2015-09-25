#include "player.h"

#include <yttrium/log.h>

namespace Yttrium
{
	OpenAlPlayer::OpenAlPlayer()
	{
		::alGenBuffers(NumBuffers, _buffers);
		if (::alGetError() == AL_NO_ERROR)
		{
			::alGenSources(1, &_source);
			if (::alGetError() == AL_NO_ERROR)
			{
				::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
				return;
			}
			::alDeleteBuffers(NumBuffers, _buffers);
		}

		Log() << "Unable to initialize OpenAlPlayer"_s;

		for (ALuint& buffer: _buffers)
			buffer = 0;
	}

	OpenAlPlayer::~OpenAlPlayer()
	{
		::alDeleteSources(1, &_source);
		::alDeleteBuffers(NumBuffers, _buffers);
	}

	bool OpenAlPlayer::set_format(const AudioFormat& format)
	{
		return _format.set(format);
	}

	void OpenAlPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		::alBufferData(_buffers[index], _format._format, data, size, _format._frequency);
		::alSourceQueueBuffers(_source, 1, &_buffers[index]);
	}

	size_t OpenAlPlayer::check_buffers()
	{
		ALint processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
		return processed;
	}

	void OpenAlPlayer::refill_buffer(void* data, size_t size)
	{
		ALuint buffer = 0;
		::alSourceUnqueueBuffers(_source, 1, &buffer);
		::alBufferData(buffer, _format._format, data, size, _format._frequency);
		::alSourceQueueBuffers(_source, 1, &buffer);
	}

	void OpenAlPlayer::play()
	{
		::alSourcePlay(_source);
	}

	void OpenAlPlayer::pause()
	{
		::alSourcePause(_source);
	}

	void OpenAlPlayer::stop()
	{
		ALint processed = 0;
		ALuint buffers[NumBuffers];

		::alSourceStop(_source);
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &processed);
		::alSourceUnqueueBuffers(_source, processed, buffers);
	}
}
