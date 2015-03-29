#include "screen.h"

#include <X11/extensions/Xrandr.h>

namespace Yttrium
{
	std::unique_ptr<ScreenImpl> ScreenImpl::open()
	{
		::Display* display = ::XOpenDisplay(nullptr);
		if (display)
		{
			int event_base;
			int error_base;
			if (::XRRQueryExtension(display, &event_base, &error_base))
				return std::make_unique<ScreenImpl>(display);
			::XCloseDisplay(display);
		}
		return {};
	}

	ScreenImpl::ScreenImpl(::Display* display)
		: _display(display)
		, _screen(DefaultScreen(display))
	{
	}

	ScreenImpl::~ScreenImpl()
	{
		::XCloseDisplay(_display);
	}

	ScreenMode ScreenImpl::current_mode() const
	{
		::XRRScreenConfiguration* config = ::XRRGetScreenInfo(_display, RootWindow(_display, _screen));

		int nsizes = 0;
		::XRRScreenSize* sizes = ::XRRConfigSizes(config, &nsizes);

		::Rotation rotation;
		const int size_index = ::XRRConfigCurrentConfiguration(config, &rotation);

		ScreenMode mode;
		mode.width = sizes[size_index].width;
		mode.height = sizes[size_index].height;
		mode.frequency = ::XRRConfigCurrentRate(config);

		::XRRFreeScreenConfigInfo(config);

		return mode;
	}

	ScreenMode ScreenImpl::default_mode() const
	{
		::XRRScreenConfiguration* config = ::XRRGetScreenInfo(_display, RootWindow(_display, _screen));

		int nsizes = 0;
		::XRRScreenSize* sizes = ::XRRConfigSizes(config, &nsizes);

		const int size_index = 0;

		int nrates = 0;
		short* rates = ::XRRConfigRates(config, size_index, &nrates);

		ScreenMode mode;
		mode.width = sizes[size_index].width;
		mode.height = sizes[size_index].height;
		mode.frequency = rates[0];

		::XRRFreeScreenConfigInfo(config);

		return mode;
	}
}
