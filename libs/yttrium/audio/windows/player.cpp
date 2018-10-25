#include "player.h"

namespace Yttrium
{
	WindowsAudioPlayer::WindowsAudioPlayer()
	{
	}

	void WindowsAudioPlayer::set_format(const AudioFormat& format)
	{
		(void)format;
	}

	void WindowsAudioPlayer::fill_buffer(size_t index, void* data, size_t size)
	{
		(void)index;
		(void)data;
		(void)size;
	}

	size_t WindowsAudioPlayer::check_buffers()
	{
		return 0;
	}

	void WindowsAudioPlayer::refill_buffer(void* data, size_t size)
	{
		(void)data;
		(void)size;
	}

	void WindowsAudioPlayer::play()
	{
	}

	void WindowsAudioPlayer::pause()
	{
	}

	void WindowsAudioPlayer::stop() noexcept
	{
	}
}
