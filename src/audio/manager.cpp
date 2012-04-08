#include "manager.hpp"

#include "openal/manager.hpp"

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

	StaticString backend_used = (!backend.is_empty() ? backend : Y_S("openal"));

	if (backend_used == Y_S("openal"))
	{
		_private = OpenAlManager::open(device, _allocator);
	}
	else
	{
		return false;
	}

	return _private;
}

AudioPlayer AudioManager::player()
{
	return AudioPlayer(&_private->_player_private);
}

AudioManager::Backends AudioManager::backends()
{
	Backends result;

	result.insert(Y_S("openal"));

	return result;
}

AudioManager::Devices AudioManager::backend_devices(const StaticString &backend)
{
	if (backend == Y_S("openal"))
	{
		return OpenAlManager::devices();
	}
	else
	{
		return Devices();
	}
}

} // namespace Yttrium
