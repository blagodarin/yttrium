#ifndef _src_audio_manager_h_
#define _src_audio_manager_h_

#include <yttrium/audio/manager.h>

#include <yttrium/audio/sound.h>
#include <yttrium/std/map.h>
#include "../base/instance_guard.h"
#include "player.h"

namespace Yttrium
{
	class AudioBackend;
	class AudioManagerImpl;

	using AudioManagerGuard = InstanceGuard<AudioManagerImpl>;

	class AudioManagerImpl : public AudioManager
	{
	public:
		static AudioManagerImpl* instance();

		AudioManagerImpl(const Storage&, const StaticString& backend, const StaticString& device, Allocator&);
		~AudioManagerImpl() override;

		StaticString backend() const override;
		SharedPtr<Sound> create_sound(const StaticString&) override;
		StaticString device() const override;
		AudioPlayer& player() override { return _player; }

		void delete_sound(const String& name);

	private:
		const Storage& _storage;
		Allocator& _allocator;
		const UniquePtr<AudioBackend> _backend;
		AudioPlayerImpl _player;
		StdMap<String, std::pair<Allocator*, Sound*>> _sounds{ _allocator };
		AudioManagerGuard _instance_guard{ this, "Duplicate AudioManager construction" };
	};
}

#endif
