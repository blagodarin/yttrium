#ifndef __AUDIO_MANAGER_H
#define __AUDIO_MANAGER_H

#include <yttrium/audio/manager.h>

#include <yttrium/audio/sound.h>
#include <yttrium/proxy_allocator.h>
#include "../base/instance_guard.h"
#include "player.h"

#include <map>

namespace Yttrium
{
	class AudioBackend;
	class AudioManagerImpl;

	using AudioManagerGuard = InstanceGuard<AudioManagerImpl>;

	class AudioManagerImpl : public AudioManager
	{
	public:

		static AudioManagerImpl* instance();

		AudioManagerImpl(const StaticString& backend, const StaticString& device, Allocator*);
		~AudioManagerImpl() override;

		StaticString backend() const override;
		StaticString device() const override;
		AudioPlayer& player() override { return _player; }

		SharedPtr<Sound> create_sound(const StaticString& name, Allocator*);
		void delete_sound(const StaticString& name);

	private:

		ProxyAllocator _allocator;
		const Pointer<AudioBackend> _backend;
		AudioPlayerImpl _player;
		std::map<String, Sound*> _sounds;
		AudioManagerGuard _instance_guard;
	};
}

#endif // __AUDIO_MANAGER_H
