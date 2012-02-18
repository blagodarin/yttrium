#include <Yttrium/assert.hpp>

#include <cstdlib> // abort

#include <Yttrium/allocator.hpp>
#include <Yttrium/logger.hpp>

namespace Yttrium
{

void abort(const StaticString &file, int line, const StaticString &function, const StaticString &message, ...) noexcept
{
	if (message)
	{
		Allocator *allocator = SystemAllocator::instance();

		// NOTE: On out of memory error we'll enter a recursive loop here.

		Logger(allocator).log(Logger::None, file, line, function) << message;
	}

	::abort();
}

} // namespace Yttrium
