#ifndef __AUDIO_MANAGER_H
#define __AUDIO_MANAGER_H

#include <Yttrium/audio/manager.h>
#include <Yttrium/audio/sound.h>

#include "player.h"

namespace Yttrium
{

class Y_PRIVATE AudioManager::Private
{
public:

	StaticString         _backend_name;
	String               _device_name;
	AudioPlayer::Private _player_private;

public:

	Private(Allocator *allocator)
		: _device_name(allocator)
		, _player_private(allocator)
		, _allocator(allocator)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual Sound::Private *create_sound() = 0;

protected:

	Allocator *_allocator;
};

} // namespace Yttrium

#endif // __AUDIO_MANAGER_H
