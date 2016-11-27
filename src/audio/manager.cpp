#include <yttrium/audio/manager.h>

#include <yttrium/audio/reader.h>
#include <yttrium/resources/resource_ptr.h>
#include "backend.h"
#include "manager.h"
#include "player.h"

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate(Allocator& allocator)
		: _allocator(allocator)
		, _backend(AudioBackend::create())
	{
	}

	std::unique_ptr<AudioPlayerBackend> AudioManagerPrivate::create_player_backend()
	{
		return _backend->create_player();
	}

	AudioManager::AudioManager(Allocator& allocator)
		: _private(std::make_unique<AudioManagerPrivate>(allocator))
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
