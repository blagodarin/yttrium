#include "screen.h"

#include <yttrium/memory/pointer.h>

#include <stdexcept>

#include <X11/extensions/Xrandr.h>

namespace Yttrium
{
	using P_XRRScreenConfiguration = Y_UNIQUE_PTR(::XRRScreenConfiguration, ::XRRFreeScreenConfigInfo);

	Pointer<ScreenImpl> ScreenImpl::open(Allocator& allocator)
	{
		P_Display display(::XOpenDisplay(nullptr));
		if (!display)
			throw std::runtime_error("Failed to open the default X11 display");

		int event_base = 0;
		int error_base = 0;
		if (!::XRRQueryExtension(display.get(), &event_base, &error_base))
			throw std::runtime_error("XRandR is unavailable");

		return make_pointer<ScreenImpl>(allocator, std::move(display));
	}

	ScreenImpl::ScreenImpl(P_Display&& display)
		: _display(std::move(display))
		, _screen(DefaultScreen(_display.get()))
	{
	}

	ScreenImpl::~ScreenImpl()
	{
	}

	ScreenMode ScreenImpl::current_mode() const
	{
		const P_XRRScreenConfiguration config(::XRRGetScreenInfo(_display.get(), RootWindow(_display.get(), _screen)));

		int nsizes = 0;
		::XRRScreenSize* sizes = ::XRRConfigSizes(config.get(), &nsizes);

		::Rotation rotation;
		const int size_index = ::XRRConfigCurrentConfiguration(config.get(), &rotation);

		ScreenMode mode;
		mode.resolution = {sizes[size_index].width, sizes[size_index].height};
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
		mode.resolution = {sizes[size_index].width, sizes[size_index].height};
		mode.frequency = rates[0];

		return mode;
	}
}
