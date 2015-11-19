/// \file
/// \brief %Screen management.

#ifndef _include_yttrium_screen_h_
#define _include_yttrium_screen_h_

#include <yttrium/math/size.h>

namespace Yttrium
{
	///
	struct ScreenMode
	{
		Size     resolution;    ///<
		unsigned frequency = 0; ///<
	};

	///
	class Screen
	{
	public:

		virtual ~Screen() = default;

		///
		virtual ScreenMode current_mode() const = 0;

		///
		virtual ScreenMode default_mode() const = 0;
	};
}

#endif
