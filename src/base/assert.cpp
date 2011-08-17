#include <cstdlib> // abort

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/logger.hpp>

namespace Yttrium
{

void abort(const StaticString &message, const char *file, int line, const char *function) throw()
{
	if (message)
	{
		Logger(SystemAllocator::instance()).log(Logger::None, file, line, function) << message;
	}
	::abort();
}

} // namespace Yttrium
