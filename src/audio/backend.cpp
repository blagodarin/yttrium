#include "backend.h"

#include <yttrium/log.h>
#include <yttrium/memory/unique_ptr.h>
#include "backend/openal/backend.h"

namespace Yttrium
{
	std::unique_ptr<AudioBackend> AudioBackend::create(Allocator& allocator)
	{
		return std::make_unique<OpenAlBackend>(allocator);
	}
}
