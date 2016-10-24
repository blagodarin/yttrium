#include "backend.h"

#include "openal/backend.h"

namespace Yttrium
{
	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<OpenAlBackend>();
	}
}
