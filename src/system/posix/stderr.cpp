#include "stderr.h"

#include <yttrium/static_string.h>

#include <unistd.h>

namespace Yttrium
{
	void StdErr::write(const StaticString& string) const
	{
		::write(STDERR_FILENO, string.text(), string.size());
	}
}
