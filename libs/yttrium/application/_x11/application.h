#pragma once

#include "../../utils/memory.h"

#include <X11/Xlib.h>

namespace Yttrium
{
	class NativeApplication
	{
	public:
		NativeApplication();

		::Display* display() const noexcept { return _display.get(); }
		int screen() const noexcept { return _screen; }

	private:
		UniquePtr<::Display, ::XCloseDisplay> _display;
		int _screen = 0;
	};
}
