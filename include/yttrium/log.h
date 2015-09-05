/// \file
/// \brief Log writer.

#ifndef __Y_LOG_H
#define __Y_LOG_H

#include <yttrium/string.h>

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
		Log& operator<<(const T& value)
		{
			_message << value;
			return *this;
		}

	private:
		String _message;
	};
}

#endif
