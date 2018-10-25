#include "backend.h"

#include "player.h"
#include "sound.h"

namespace Yttrium
{
	WindowsAudioBackend::WindowsAudioBackend()
	{
	}

	WindowsAudioBackend::~WindowsAudioBackend()
	{
	}

	std::unique_ptr<AudioPlayerBackend> WindowsAudioBackend::create_player()
	{
		return std::make_unique<WindowsAudioPlayer>();
	}

	std::unique_ptr<Sound> WindowsAudioBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<WindowsSound>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<WindowsAudioBackend>();
	}
}
