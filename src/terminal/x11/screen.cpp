#include "screen.h"

#include <X11/extensions/Xrandr.h>

namespace Yttrium
{

ScreenX11::ScreenX11(::Display* display, int screen, Allocator* allocator)
	: Screen(allocator)
	, _display(display)
	, _screen(screen)
{
}

ScreenX11::~ScreenX11()
{
	::XCloseDisplay(_display);
}

ScreenMode ScreenX11::mode(ModeType type)
{
	::XRRScreenConfiguration* config = ::XRRGetScreenInfo(_display, RootWindow(_display, _screen));

	int nsizes = 0;
	::XRRScreenSize* sizes = ::XRRConfigSizes(config, &nsizes);

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
		result.width = sizes[0].width;
		result.height = sizes[0].height;

		int nrates = 0;
		result.frequency = ::XRRConfigRates(config, 0, &nrates)[0];
	}

	result.bits_per_pixel = 0;

	::XRRFreeScreenConfigInfo(config);

	return result;
}

ScreenPtr Screen::open(Allocator* allocator)
{
	::Display* display = ::XOpenDisplay(nullptr);
	if (display)
	{
		int event_base;
		int error_base;
		if (::XRRQueryExtension(display, &event_base, &error_base))
			return ScreenPtr(Y_NEW(allocator, ScreenX11)(display, DefaultScreen(display), allocator));
		::XCloseDisplay(display);
	}
	return ScreenPtr();
}

} // namespace Yttrium
