#ifndef _src_window_x11_screen_h_
#define _src_window_x11_screen_h_

#include <yttrium/screen.h>
#include "../../utils/unique_ptr.h"

#include <X11/Xlib.h>

namespace Yttrium
{
	using P_Display = Y_UNIQUE_PTR(::Display, ::XCloseDisplay);

	class ScreenImpl : public Screen
	{
	public:
		ScreenImpl();
		~ScreenImpl() override = default;

		ScreenMode current_mode() const override;
		ScreenMode default_mode() const override;

		::Display* display() const { return _display.get(); }
		int screen() const { return _screen; }

	private:
		const P_Display _display;
		const int _screen = DefaultScreen(_display.get());
	};
}

#endif
