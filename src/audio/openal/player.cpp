#include "player.h"

namespace Yttrium
{
	OpenAlPlayer::OpenAlPlayer()
	{
		::alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void OpenAlPlayer::set_format(const AudioFormat& format)
	{
		_format = OpenALFormat(format);
	}

	void OpenAlPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		::alBufferData(_buffers[index], _format._format, data, size, _format._frequency);
		::alSourceQueueBuffers(_source, 1, &_buffers[index]);
	}

	size_t OpenAlPlayer::check_buffers()
	{
		ALint buffers_processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffers_processed);
		return static_cast<size_t>(buffers_processed);
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

	void OpenAlPlayer::stop() noexcept
	{
		::alSourceStop(_source);

		ALint buffers_processed = 0;
		::alGetSourcei(_source, AL_BUFFERS_PROCESSED, &buffers_processed);

		ALuint buffers[2];
		::alSourceUnqueueBuffers(_source, buffers_processed, buffers);
	}
}
