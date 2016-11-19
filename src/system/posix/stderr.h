#ifndef _src_system_posix_stderr_h_
#define _src_system_posix_stderr_h_

namespace Yttrium
{
	class StaticString;

	class StdErr
	{
	public:
		void write(const StaticString&) const;
	};
}

#endif
