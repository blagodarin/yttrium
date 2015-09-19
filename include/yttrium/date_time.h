/// \file
/// \brief Date and time.

#ifndef _include_yttrium_date_time_h_
#define _include_yttrium_date_time_h_

#include <yttrium/global.h>

#include <cstdint>

namespace Yttrium
{
	/// Date and time representation.
	struct Y_API DateTime
	{
		uint_fast16_t year;    ///< Year.
		uint_fast8_t  month;   ///< Month (1 - 12).
		uint_fast8_t  day;     ///< Day (1 - 31).
		uint_fast8_t  hour;    ///< Hour (0 - 23).
		uint_fast8_t  minute;  ///< Minute.
		uint_fast8_t  second;  ///< Second.
		uint_fast16_t msecond; ///< Millisecond.

		/// Get the current date and time.
		static DateTime now();
	};
}

#endif
