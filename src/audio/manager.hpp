#ifndef __AUDIO_MANAGER_HPP
#define __AUDIO_MANAGER_HPP

#include <Yttrium/audio/manager.hpp>
#include <Yttrium/logger.hpp>

#include "player.hpp"

namespace Yttrium
{

class AudioManager::Private
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

#endif // __AUDIO_MANAGER_HPP
