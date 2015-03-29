/// \file
/// \brief %Screen management.

#ifndef __Y_SCREEN_H
#define __Y_SCREEN_H

#include <yttrium/global.h>

#include <memory>

namespace Yttrium
{
	///
	struct ScreenMode
	{
		int      width = 0;          ///<
		int      height = 0;         ///<
		unsigned bits_per_pixel = 0; ///<
		unsigned frequency = 0;      ///<
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

#endif // __Y_SCREEN_H
