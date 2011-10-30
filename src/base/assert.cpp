#include <cstdlib> // abort

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/logger.hpp>

namespace Yttrium
{

void abort(const StaticString &message, const StaticString &file, int line, const StaticString &function) throw()
{
	if (message)
	{
		Logger(SystemAllocator::instance()).log(Logger::None, file, line, function) << message;
	}
	::abort();
}

} // namespace Yttrium
