/// \file
/// \brief Log writer.

#ifndef _include_yttrium_log_h_
#define _include_yttrium_log_h_

#include <yttrium/string.h>
#include <yttrium/string_format.h>

namespace Yttrium
{
	///
	class Y_API Log
	{
	public:

		Log();
		~Log();

		///
		template <typename T>
		Log& operator<<(T&& value)
		{
			_message << std::forward<T>(value);
			return *this;
		}

	private:
		String _message;
	};
}

#endif
