#include "player.h"

#include <Yttrium/allocator.h>

#include "openal/player.h"

namespace Yttrium
{

AudioPlayerBackend *AudioPlayerBackend::create(Allocator *allocator)
{
	return Y_NEW(allocator, OpenAlPlayer);
}

} // namespace Yttrium
