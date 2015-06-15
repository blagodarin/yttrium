#ifndef __WINDOW_X11_SCREEN_H
#define __WINDOW_X11_SCREEN_H

#include <yttrium/screen.h>

#include "../../base/private_base.h"
#include "../../base/utils.h"

#include <X11/Xlib.h>

namespace Yttrium
{
	template <typename> class Pointer;

	using P_Display = Y_UNIQUE_PTR(::Display, ::XCloseDisplay);

	class ScreenImpl : public Screen
	{
	public:

		static Pointer<ScreenImpl> open(Allocator&);

		ScreenImpl(P_Display display);
		~ScreenImpl() override;

		ScreenMode current_mode() const override;
		ScreenMode default_mode() const override;

		::Display* display() const { return _display.get(); }
		int screen() const { return _screen; }

	private:

		const P_Display _display;
		const int _screen;
	};
}

#endif // __WINDOW_X11_SCREEN_H
