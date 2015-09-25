#include "backend.h"

#include <yttrium/pointer.h>
#include "backend/openal/backend.h"

namespace Yttrium
{
	const StaticString AudioBackend::OpenAL = "openal"_s;

	Pointer<AudioBackend> AudioBackend::create(const StaticString& backend, const StaticString& device, Allocator* allocator)
	{
		const StaticString actual_backend = !backend.is_empty() ? backend : OpenAL;
		if (actual_backend == OpenAL)
			return OpenAlBackend::create(device, allocator);
		throw std::runtime_error("Audio: Unknown backend \"" + std::string(backend.text(), backend.size()) + "\"");
	}
}
