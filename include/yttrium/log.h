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

		///
		static void set_file(const StaticString&);

		Log();
		~Log();

		///
		template <typename T>
		Y_PRIVATE Log& operator<<(T&& value)
		{
			_message << std::forward<T>(value);
			return *this;
		}

	private:
		String _message;
	};
}

#endif
