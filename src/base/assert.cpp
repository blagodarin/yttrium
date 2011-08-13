#include <cstdlib> // abort

#include <Yttrium/allocators.hpp>
#include <Yttrium/assert.hpp>
#include <Yttrium/logger.hpp>

namespace Yttrium
{

void abort(const char *message) throw()
{
	if (message)
	{
		Logger logger(SystemAllocator::instance());

		Y_LOG(logger, message);
	}
	::abort();
}

} // namespace Yttrium
