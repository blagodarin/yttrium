#include "manager.h"

#include <yttrium/proxy_allocator.h>
#include "backend/manager.h"
#include "sound.h"

namespace Yttrium
{
	AudioManager::Private::Private(Allocator* allocator, const StaticString& device_name)
		: _instance_guard(this, "Duplicate AudioManager construction")
		, _allocator(allocator)
		, _device_name(device_name, allocator)
		, _player_private(allocator)
	{
	}

	AudioManager::Private::~Private()
	{
		Y_ASSERT(_sounds.empty());
	}

	AudioManager::Private* AudioManager::Private::instance()
	{
		return AudioManagerGuard::instance;
	}

	AudioManager::AudioManager(Allocator* allocator)
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

	bool AudioManager::open(const StaticString& backend, const StaticString& device)
	{
		close();

		_private = open_audio_manager(backend, device, _allocator);

		return _private;
	}

	AudioPlayer AudioManager::player()
	{
		return AudioPlayer(&_private->_player_private);
	}
}
