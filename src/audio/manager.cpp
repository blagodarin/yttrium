#include <yttrium/audio/manager.h>

#include <yttrium/audio/reader.h>
#include <yttrium/resources/resource_ptr.h>
#include "backend.h"
#include "player.h"

namespace Yttrium
{
	class AudioManagerPrivate
	{
	public:
		AudioManagerPrivate(Allocator& allocator)
			: _allocator(allocator)
		{
		}

	public:
		Allocator& _allocator;
		const std::unique_ptr<AudioBackend> _backend = AudioBackend::create();
		AudioPlayerImpl _player{ _backend->create_player(), _allocator };
	};

	AudioManager::AudioManager(Allocator& allocator)
		: _private(std::make_unique<AudioManagerPrivate>(allocator))
	{
	}

	ResourcePtr<Sound> AudioManager::create_sound(Reader&& reader)
	{
		const auto audio_reader = AudioReader::open(std::move(reader));
		if (!audio_reader)
			return {};
		return _private->_backend->create_sound(*audio_reader);
	}

	AudioPlayer& AudioManager::player()
	{
		return _private->_player;
	}

	AudioManager::~AudioManager() = default;
}
