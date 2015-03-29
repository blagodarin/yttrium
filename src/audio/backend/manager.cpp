#include "manager.h"

#include "openal/manager.h"

namespace Yttrium
{
	const S backend_openal("openal");

	std::vector<StaticString> AudioManager::backends()
	{
		std::vector<StaticString> result;
		result.emplace_back(backend_openal);
		return result;
	}

	std::vector<StaticString> AudioManager::backend_devices(const StaticString& backend)
	{
		if (backend == backend_openal)
			return OpenAlManager::devices();
		else
			return std::vector<StaticString>();
	}

	AudioManager::Private* open_audio_manager(const StaticString& backend, const StaticString& device, Allocator* allocator)
	{
		AudioManager::Private* result = nullptr;

		StaticString backend_used = !backend.is_empty() ? backend : backend_openal;

		// Don't assign 'backend_used' to 'result->_backend_name',
		// it may be 'backend' which may be mutable on the caller side.

		if (backend_used == backend_openal)
		{
			result = OpenAlManager::open(device, allocator);
			if (result)
				result->_backend_name = backend_openal;
		}

		return result;
	}
}
