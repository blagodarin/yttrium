#include "player.h"

namespace Yttrium
{
	OpenAlPlayer::OpenAlPlayer()
	{
		_source.set_int(AL_SOURCE_RELATIVE, AL_TRUE);
	}

	void OpenAlPlayer::set_format(const AudioFormat& format)
	{
		_format = OpenALFormat(format);
	}

	void OpenAlPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		::alBufferData(_buffers[index], _format._format, data, size, _format._frequency);
		::alSourceQueueBuffers(_source.get(), 1, &_buffers[index]);
	}

	size_t OpenAlPlayer::check_buffers()
	{
		return _source.get_int(AL_BUFFERS_PROCESSED);
	}

	void OpenAlPlayer::refill_buffer(void* data, size_t size)
	{
		ALuint buffer = 0;
		::alSourceUnqueueBuffers(_source.get(), 1, &buffer);
		::alBufferData(buffer, _format._format, data, size, _format._frequency);
		::alSourceQueueBuffers(_source.get(), 1, &buffer);
	}

	void OpenAlPlayer::play()
	{
		::alSourcePlay(_source.get());
	}

	void OpenAlPlayer::pause()
	{
		::alSourcePause(_source.get());
	}

	void OpenAlPlayer::stop()
	{
		ALuint buffers[NumBuffers];

		::alSourceStop(_source.get());
		::alSourceUnqueueBuffers(_source.get(), _source.get_int(AL_BUFFERS_PROCESSED), buffers);
	}
}
