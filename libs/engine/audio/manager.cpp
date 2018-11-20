#include <yttrium/audio/manager.h>

#include <yttrium/audio/reader.h>
#include <yttrium/audio/sound.h>
#include "backend.h"
#include "manager.h"
#include "player.h"

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate()
		: _backend{ AudioBackend::create() }
	{
	}

	AudioManager::AudioManager()
		: _private{ std::make_unique<AudioManagerPrivate>() }
	{
	}

	AudioManager::~AudioManager() noexcept = default;

	std::unique_ptr<Sound> AudioManager::create_sound(std::unique_ptr<Source>&& source)
	{
		const auto reader = AudioReader::open(std::move(source));
		return reader ? _private->_backend->create_sound(*reader) : nullptr;
	}
}
