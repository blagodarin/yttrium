#include <yttrium/audio/manager.h>

#include <yttrium/audio/reader.h>
#include <yttrium/resources/resource_ptr.h>
#include "backend.h"
#include "manager.h"
#include "player.h"

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate()
		: _backend(AudioBackend::create())
	{
	}

	AudioManager::AudioManager()
		: _private(std::make_unique<AudioManagerPrivate>())
	{
	}

	AudioManager::~AudioManager() = default;

	ResourcePtr<Sound> AudioManager::create_sound(Reader&& reader)
	{
		const auto audio_reader = AudioReader::open(std::move(reader));
		if (!audio_reader)
			return {};
		return _private->_backend->create_sound(*audio_reader);
	}
}
