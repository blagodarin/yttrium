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
		using DisplayPtr = UniquePtr<::Display, ::XCloseDisplay>;

		static DisplayPtr open_display();

	private:
		const DisplayPtr _display = open_display();
		const int _screen = DefaultScreen(_display.get());
	};
}
