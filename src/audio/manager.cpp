#include <yttrium/audio/manager.h>

#include <yttrium/audio/io.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>
#include <yttrium/resources/resource_ptr.h>
#include "backend.h"
#include "player.h"

namespace Yttrium
{
	class AudioManagerPrivate
	{
	public:
		AudioManagerPrivate(const Storage& storage, Allocator& allocator)
			: _storage(storage)
			, _allocator(allocator)
		{
		}

	public:
		const Storage& _storage;
		Allocator& _allocator;
		const std::unique_ptr<AudioBackend> _backend = AudioBackend::create();
		AudioPlayerImpl _player{ _storage, _backend->create_player(), _allocator };
	};

	AudioManager::AudioManager(const Storage& storage, Allocator& allocator)
		: _private(std::make_unique<AudioManagerPrivate>(storage, allocator))
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
