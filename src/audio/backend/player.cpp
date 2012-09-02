#include "player.h"

#include <Yttrium/allocator.h>

#include "openal/player.h"

namespace Yttrium
{

AudioPlayerBackend *AudioPlayerBackend::create(Allocator *allocator)
{
	return new(allocator->allocate<OpenAlPlayer>())
		OpenAlPlayer();
}

} // namespace Yttrium
