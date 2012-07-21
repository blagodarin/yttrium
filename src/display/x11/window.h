#ifndef __DISPLAY_X11_WINDOW_H
#define __DISPLAY_X11_WINDOW_H

#include <Yttrium/window.h>

#include "../../base/private_base.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Yttrium
{

class Y_PRIVATE Window::Private: public PrivateBase<Window::Private>
{
public:

	::Display  *_display;
	int         _screen;
	::Window    _window;
	::Atom      _wm_protocols;
	::Atom      _wm_delete_window;
	GLXContext  _glx_context;

public:

	Private(::Display *display, int screen, ::Window window, ::GLXContext glx_context, Allocator *allocator);

	~Private();

public:

	void close();

	void set_size(int width, int height);

public:

	static bool create_window(::Display *display, int screen, ::Window *window, ::GLXContext *glx_context);

	static Key decode_key(::XEvent &event);
};

} // namespace Yttrium

#endif // __DISPLAY_X11_WINDOW_H
