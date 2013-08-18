#ifndef __AUDIO_MANAGER_H
#define __AUDIO_MANAGER_H

#include <yttrium/audio/manager.h>
#include <yttrium/audio/sound.h>

#include "player.h"

#include <map>

namespace Yttrium
{

class SoundImpl;

class Y_PRIVATE AudioManager::Private
{
public:
	
	typedef std::map<String, Sound *> Sounds;

	Allocator            *_allocator;
	StaticString          _backend_name;
	String                _device_name;
	AudioPlayer::Private  _player_private;
	Sounds                _sounds;

public:

	Private(Allocator *allocator);

	virtual ~Private();

public:

	virtual SoundImpl *create_sound(const StaticString &name, Allocator *allocator) = 0;

public:

	static Private *instance();
};

} // namespace Yttrium

#endif // __AUDIO_MANAGER_H
