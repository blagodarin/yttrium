#include <Yttrium/assert.h>

#include <cstdlib> // abort

#include <Yttrium/allocator.h>
#include <Yttrium/logger.h>

namespace Yttrium
{

void abort(const StaticString &file, int line, const StaticString &function, const StaticString &message, ...)
{
	if (!message.is_empty())
	{
		Allocator *allocator = SystemAllocator::instance();

		// NOTE: If we're aborting on an out of memory error and the message size
		// exceeds the allocated string capacity, we'll enter a recursive loop here.

		Logger(allocator).log(Logger::None, file, line, function) << message;
	}

	::abort();
}

} // namespace Yttrium
