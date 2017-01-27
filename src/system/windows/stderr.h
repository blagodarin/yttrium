#ifndef _src_system_windows_stderr_h_
#define _src_system_windows_stderr_h_

#include "windows.h"

namespace Yttrium
{
	class StaticString;

	class StdErr
	{
	public:
		StdErr();
		void write(const StaticString&) const;
	private:
		const HANDLE _handle;
	};
}

#endif
