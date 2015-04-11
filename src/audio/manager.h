#ifndef __AUDIO_MANAGER_H
#define __AUDIO_MANAGER_H

#include <yttrium/audio/manager.h>

#include <yttrium/audio/sound.h>
#include "../base/instance_guard.h"
#include "player.h"

#include <map>

namespace Yttrium
{
	class SoundImpl;

	typedef InstanceGuard<AudioManager::Private> AudioManagerGuard;

	class Y_PRIVATE AudioManager::Private
	{
	public:

		AudioManagerGuard        _instance_guard;
		Allocator*               _allocator;
		StaticString             _backend_name;
		String                   _device_name;
		AudioPlayer::Private     _player_private;
		std::map<String, Sound*> _sounds;

	public:

		Private(Allocator* allocator, const StaticString& device_name);
		virtual ~Private();

	public:

		virtual SoundImpl* create_sound(const StaticString& name, Allocator* allocator) = 0;

	public:

		static Private* instance();
	};
}

#endif // __AUDIO_MANAGER_H
