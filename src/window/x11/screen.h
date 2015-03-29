#ifndef __WINDOW_X11_SCREEN_H
#define __WINDOW_X11_SCREEN_H

#include <yttrium/screen.h>

#include "../../base/private_base.h"

#include <X11/Xlib.h>

namespace Yttrium
{
	class ScreenImpl: public Screen
	{
	public:

		static std::unique_ptr<ScreenImpl> open();

		ScreenImpl(::Display* display);
		~ScreenImpl() override;

		ScreenMode current_mode() const override;
		ScreenMode default_mode() const override;

		::Display* display() const { return _display; }
		int screen() const { return _screen; }

	private:

		::Display* _display;
		int        _screen;
	};
}

#endif // __WINDOW_X11_SCREEN_H
