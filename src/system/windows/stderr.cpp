#include "stderr.h"

#include <yttrium/static_string.h>

namespace Yttrium
{
	StdErr::StdErr()
		: _handle(::GetStdHandle(STD_ERROR_HANDLE))
	{
	}

	void StdErr::write(const StaticString& string) const
	{
		DWORD written = 0;
		::WriteFile(_handle, string.text(), string.size(), &written, nullptr);
	}
}
