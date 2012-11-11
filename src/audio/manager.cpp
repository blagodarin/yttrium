#include "manager.h"

#include "backend/manager.h"

namespace Yttrium
{

StaticString AudioManager::backend() const
{
	return _private ? _private->_backend_name : StaticString();
}

void AudioManager::close()
{
	Y_DELETE(_allocator, _private);
	_private = nullptr;
}

StaticString AudioManager::device() const
{
	return _private ? _private->_device_name : StaticString();
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
