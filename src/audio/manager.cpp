#include "manager.h"

#include <Yttrium/proxy_allocator.h>

#include "backend/manager.h"

namespace Yttrium
{

AudioManager::AudioManager(Allocator *allocator)
	: _allocator(Y_NEW(allocator, ProxyAllocator)("audio", allocator))
	, _private(nullptr)
{
}

AudioManager::~AudioManager()
{
	close();
	Y_DELETE(_allocator->allocator(), _allocator);
}

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
