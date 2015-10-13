#ifndef _src_window_x11_screen_h_
#define _src_window_x11_screen_h_

#include <yttrium/screen.h>

#include "../../base/private_base.h"
#include "../../utils/unique_ptr.h"

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

#endif
