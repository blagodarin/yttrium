#ifndef __WINDOW_X11_SCREEN_H
#define __WINDOW_X11_SCREEN_H

#include <yttrium/screen.h>

#include "../../base/private_base.h"

#include <X11/Xlib.h>

namespace Yttrium
{

class WindowBackend;

class ScreenImpl: public Screen
{
	friend WindowBackend;

public:

	ScreenImpl(::Display* display, int screen, Allocator* allocator);
	~ScreenImpl() override;

	ScreenMode mode(ModeType type) override;

private:

	::Display* _display;
	int        _screen;
};

} // namespace Yttrium

#endif // __WINDOW_X11_SCREEN_H
