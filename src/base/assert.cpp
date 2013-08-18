#include <yttrium/assert.h>

#include <yttrium/log.h>

#include <cstdlib>

namespace Yttrium
{

void abort(const char *message, const char *file, int line, const char *function)
{
	// NOTE: If we're aborting on an out of memory error and the message size
	// exceeds the allocated string capacity, we'll enter a recursive loop here.

	Y_LOG(message << " {" << file << ":" << line << " | " << function << "}");

	::abort();
}

} // namespace Yttrium
