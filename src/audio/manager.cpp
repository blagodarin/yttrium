#include "manager.h"

#include <yttrium/io/reader.h>
#include <yttrium/io/storage.h>
#include <yttrium/resources/resource_ptr.h>
#include "sound.h"

#include "backend/openal/backend.h"

namespace Yttrium
{
	AudioManagerPrivate::AudioManagerPrivate(const Storage& storage, Allocator& allocator)
		: _storage(storage)
		, _allocator(allocator)
		, _backend(AudioBackend::create(_allocator))
		, _player(_storage, _backend->create_player(), _allocator)
	{
	}

	AudioManager::AudioManager(const Storage& storage, Allocator& allocator)
		: _private(std::make_unique<AudioManagerPrivate>(storage, allocator))
	{
	}

	ResourcePtr<Sound> AudioManager::create_sound(Reader&& reader)
	{
		const auto audio_reader = AudioReader::open(std::move(reader), AudioType::Auto, _private->_allocator);
		if (!audio_reader)
			return {};
		auto sound = _private->_backend->create_sound();
		if (!sound->load(*audio_reader))
			return {};
		return sound;
	}

	AudioPlayer& AudioManager::player()
	{
		return _private->_player;
	}

	AudioManager::~AudioManager() = default;
}
