#include "application.h"

#include <yttrium/exceptions.h>

namespace Yttrium
{
	NativeApplication::NativeApplication()
	{
		_display.reset(::XOpenDisplay(nullptr));
		if (!_display)
			throw InitializationError{ "XOpenDisplay failed" };
		_screen = DefaultScreen(_display.get());
	}
}
