#include "manager.h"

#include "backend/manager.h"

namespace Yttrium
{

void AudioManager::close()
{
	_allocator->delete_(_private);
	_private = nullptr;
}

bool AudioManager::open(const StaticString &backend, const StaticString &device)
{
	close();

	_private = open_audio_manager(backend, device, _allocator);

	return _private;
}

AudioPlayer AudioManager::player()
{
	return AudioPlayer(&_private->_player_private);
}

} // namespace Yttrium
