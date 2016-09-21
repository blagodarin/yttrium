#include "screen.h"

#include <stdexcept>

#include <X11/extensions/Xrandr.h>

namespace Yttrium
{
	namespace
	{
		using P_XRRScreenConfiguration = Y_UNIQUE_PTR(::XRRScreenConfiguration, ::XRRFreeScreenConfigInfo);

		P_Display open_display()
		{
			P_Display display(::XOpenDisplay(nullptr));
			if (!display)
				throw std::runtime_error("Failed to open the default X11 display");
			return display;
		}
	}

	ScreenImpl::ScreenImpl()
		: _display(open_display())
	{
		int event_base = 0;
		int error_base = 0;
		if (!::XRRQueryExtension(_display.get(), &event_base, &error_base))
			throw std::runtime_error("XRandR is unavailable");
	}

	ScreenMode ScreenImpl::current_mode() const
	{
		const P_XRRScreenConfiguration config(::XRRGetScreenInfo(_display.get(), RootWindow(_display.get(), _screen)));

		int nsizes = 0;
		::XRRScreenSize* sizes = ::XRRConfigSizes(config.get(), &nsizes);

		::Rotation rotation;
		const int size_index = ::XRRConfigCurrentConfiguration(config.get(), &rotation);

		ScreenMode mode;
		mode.resolution = { sizes[size_index].width, sizes[size_index].height };
		mode.frequency = ::XRRConfigCurrentRate(config.get());

		return mode;
	}

	ScreenMode ScreenImpl::default_mode() const
	{
		const P_XRRScreenConfiguration config(::XRRGetScreenInfo(_display.get(), RootWindow(_display.get(), _screen)));

		int nsizes = 0;
		::XRRScreenSize* sizes = ::XRRConfigSizes(config.get(), &nsizes);

		const int size_index = 0;

		int nrates = 0;
		short* rates = ::XRRConfigRates(config.get(), size_index, &nrates);

		ScreenMode mode;
		mode.resolution = { sizes[size_index].width, sizes[size_index].height };
		mode.frequency = rates[0];

		return mode;
	}
}
