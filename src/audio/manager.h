#ifndef __AUDIO_MANAGER_H
#define __AUDIO_MANAGER_H

#include <Yttrium/audio/manager.h>
#include <Yttrium/logger.h>

#include "player.h"

namespace Yttrium
{

class Y_PRIVATE AudioManager::Private
{
public:

	Private(Allocator *allocator)
		: _player_private(allocator)
		, _allocator(allocator)
		, _logger(Y_S("audio.manager"), allocator)
	{
	}

	virtual ~Private()
	{
	}

public:

	AudioPlayer::Private _player_private;

protected:

	Allocator *_allocator;
	Logger     _logger;
};

} // namespace Yttrium

#endif // __AUDIO_MANAGER_H
