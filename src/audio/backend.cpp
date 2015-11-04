#include "backend.h"

#include <yttrium/log.h>
#include <yttrium/pointer.h>
#include "backend/openal/backend.h"

namespace Yttrium
{
	const StaticString AudioBackend::OpenAL = "openal"_s;

	Pointer<AudioBackend> AudioBackend::create(const StaticString& backend, const StaticString& device, Allocator& allocator)
	{
		const StaticString actual_backend = !backend.is_empty() ? backend : OpenAL;
		if (actual_backend == OpenAL)
			return OpenAlBackend::create(device, allocator);
		throw UnableToCreate(String(&allocator) << "(audio) Unknown backend \""_s << backend << "\""_s);
	}

	AudioBackend::AudioBackend(const StaticString& backend, const StaticString& device, Allocator& allocator)
		: _backend(backend, &allocator)
		, _device(device, &allocator)
	{
		Log() << "(audio/"_s << backend << ") Using \""_s << device << "\""_s;
	}
}
