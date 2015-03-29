#include "player.h"

#include <yttrium/allocator.h>

#include "openal/player.h"

namespace Yttrium
{
	AudioPlayerBackend* AudioPlayerBackend::create(Allocator* allocator)
	{
		return Y_NEW(allocator, OpenAlPlayer);
	}
}
