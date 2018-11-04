#include "application.h"

#include <yttrium/exceptions.h>

namespace Yttrium
{
	NativeApplication::NativeApplication() = default;

	NativeApplication::DisplayPtr NativeApplication::open_display()
	{
		DisplayPtr display{ ::XOpenDisplay(nullptr) };
		if (!display)
			throw InitializationError{ "XOpenDisplay failed" };
		return display;
	}
}
