#include "manager.hpp"

#include "openal/manager.hpp"

namespace Yttrium
{

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

AudioManager::Private *open_audio_manager(const StaticString &backend, const StaticString &device, Allocator *allocator)
{
	AudioManager::Private *result = nullptr;

	StaticString backend_used = (!backend.is_empty() ? backend : Y_S("openal"));

	if (backend_used == Y_S("openal"))
	{
		result = OpenAlManager::open(device, allocator);
	}

	return result;
}

} // namespace Yttrium
