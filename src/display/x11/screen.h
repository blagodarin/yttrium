#ifndef __DISPLAY_X11_SCREEN_H
#define __DISPLAY_X11_SCREEN_H

#include <Yttrium/screen.h>

#include "../../base/private_base.h"

#include <X11/Xlib.h>

namespace Yttrium
{

class Y_PRIVATE Screen::Private: public PrivateBase<Screen::Private>
{
public:

	::Display *_display;
	int        _screen;

public:

	Private(::Display *display, int screen, Allocator *allocator);

	~Private();
};

} // namespace Yttrium

#endif // __DISPLAY_X11_SCREEN_H
