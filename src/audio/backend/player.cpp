#include "player.hpp"

#include "openal/player.hpp"

namespace Yttrium
{

AudioPlayerBackend *AudioPlayerBackend::create(Allocator *allocator)
{
	return new(allocator->allocate<OpenAlPlayer>())
		OpenAlPlayer();
}

} // namespace Yttrium
