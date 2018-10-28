#include "player.h"

namespace Yttrium
{
	WasapiAudioPlayer::WasapiAudioPlayer()
	{
	}

	void WasapiAudioPlayer::set_format(const AudioFormat& format)
	{
		(void)format;
	}

	void WasapiAudioPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		(void)index;
		(void)data;
		(void)size;
	}

	size_t WasapiAudioPlayer::check_buffers()
	{
		return 0;
	}

	void WasapiAudioPlayer::refill_buffer(void* data, size_t size)
	{
		(void)data;
		(void)size;
	}

	void WasapiAudioPlayer::play()
	{
	}

	void WasapiAudioPlayer::pause()
	{
	}

	void WasapiAudioPlayer::stop() noexcept
	{
	}
}
