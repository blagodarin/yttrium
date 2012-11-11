#include "screen.h"

#include <X11/extensions/Xrandr.h>

namespace Yttrium
{

Screen::Private::Private(::Display *display, int screen, Allocator *allocator)
	: PrivateBase(allocator)
	, _display(display)
	, _screen(screen)
{
}

Screen::Private::~Private()
{
	::XCloseDisplay(_display);
}

bool Screen::open(Allocator *allocator)
{
	close();

	::Display *display = ::XOpenDisplay(nullptr);

	if (display)
	{
		int event_base;
		int error_base;

		if (!::XRRQueryExtension(display, &event_base, &error_base))
		{
			::XCloseDisplay(display);
		}
		else
		{
			_private = Y_NEW(allocator, Private)(display, DefaultScreen(display), allocator);
			return true;
		}
	}

	return false;
}

ScreenMode Screen::mode(ModeType type)
{
	::XRRScreenConfiguration *config = ::XRRGetScreenInfo(_private->_display,
		RootWindow(_private->_display, _private->_screen));

	int nsizes;

	::XRRScreenSize *sizes = ::XRRConfigSizes(config, &nsizes);

	ScreenMode result;

	if (type != DefaultMode)
	{
		::Rotation rotation;

		::SizeID size_id = ::XRRConfigCurrentConfiguration(config, &rotation);
		result.width = sizes[size_id].width;
		result.height = sizes[size_id].height;
		result.frequency = ::XRRConfigCurrentRate(config);
	}
	else
	{
		int nrates;

		result.width = sizes[0].width;
		result.height = sizes[0].height;
		result.frequency = ::XRRConfigRates(config, 0, &nrates)[0];
	}

	result.bits_per_pixel = 0;

	::XRRFreeScreenConfigInfo(config);

	return result;
}

} // namespace Yttrium
