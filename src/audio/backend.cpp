#include "backend.h"

#include "backend/openal/backend.h"

namespace Yttrium
{
	std::unique_ptr<AudioBackend> AudioBackend::create(Allocator& allocator)
	{
		return std::make_unique<OpenAlBackend>(allocator);
	}
}
