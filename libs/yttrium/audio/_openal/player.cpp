#include "player.h"

namespace Yttrium
{
	OpenALPlayer::OpenALPlayer()
	{
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void OpenALPlayer::set_format(const AudioFormat& format)
	{
		_format = OpenALFormat(format);
	}

	void OpenALPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		::alBufferData(_buffers[index], _format._format, data, static_cast<ALsizei>(size), _format._frequency);
		::alSourceQueueBuffers(_source, 1, &_buffers[index]);
	}

	size_t OpenALPlayer::check_buffers()
	{
		ALint buffers_processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffers_processed);
		return static_cast<size_t>(buffers_processed);
	}

	void OpenALPlayer::refill_buffer(void* data, size_t size)
	{
		ALuint buffer = 0;
		::alSourceUnqueueBuffers(_source, 1, &buffer);
		::alBufferData(buffer, _format._format, data, static_cast<ALsizei>(size), _format._frequency);
		::alSourceQueueBuffers(_source, 1, &buffer);
	}

	void OpenALPlayer::play()
	{
		::alSourcePlay(_source);
	}

	void OpenALPlayer::pause()
	{
		::alSourcePause(_source);
	}

	void OpenALPlayer::stop() noexcept
	{
		::alSourceStop(_source);

		ALint buffers_processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffers_processed);

		ALuint buffers[2];
		::alSourceUnqueueBuffers(_source, buffers_processed, buffers);
	}
}
