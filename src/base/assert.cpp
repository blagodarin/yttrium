#include <Yttrium/assert.h>

#include <Yttrium/log.h>

#include <cstdlib>

namespace Yttrium
{

void abort(const StaticString &message, ...)
{
	if (!message.is_empty())
	{
		// NOTE: If we're aborting on an out of memory error and the message size
		// exceeds the allocated string capacity, we'll enter a recursive loop here.

		Y_LOG(message);
	}

	::abort();
}

} // namespace Yttrium
