#ifndef _src_audio_manager_h_
#define _src_audio_manager_h_

#include <yttrium/audio/manager.h>

#include <yttrium/audio/sound.h>
#include <yttrium/std/map.h>
#include "player.h"

namespace Yttrium
{
	class AudioBackend;
	class AudioManagerImpl;

	class AudioManagerImpl : public AudioManager
	{
	public:
		AudioManagerImpl(const Storage&, const StaticString& backend, const StaticString& device, Allocator&);

		StaticString backend() const override;
		ResourcePtr<Sound> create_sound(const StaticString&) override;
		StaticString device() const override;
		AudioPlayer& player() override { return _player; }

	private:
		const Storage& _storage;
		Allocator& _allocator;
		const UniquePtr<AudioBackend> _backend;
		AudioPlayerImpl _player;
	};
}

#endif
