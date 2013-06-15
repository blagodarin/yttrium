#include "manager.h"

#include "openal/manager.h"

namespace Yttrium
{

const S backend_openal("openal");

AudioManager::Backends AudioManager::backends()
{
	Backends result;

	result.insert(backend_openal);

	return result;
}

AudioManager::Devices AudioManager::backend_devices(const StaticString &backend)
{
	if (backend == backend_openal)
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

	StaticString backend_used = (!backend.is_empty() ? backend : backend_openal);

	// Don't assign 'backend_used' to 'result->backend',
	// it may be 'backend' which may be mutable on the caller side.

	if (backend_used == backend_openal)
	{
		result = OpenAlManager::open(device, allocator);

		if (result)
		{
			result->_backend_name = backend_openal;
		}
	}

	return result;
}

} // namespace Yttrium
