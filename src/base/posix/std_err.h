#ifndef _src_base_posix_std_err_h_
#define _src_base_posix_std_err_h_

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
