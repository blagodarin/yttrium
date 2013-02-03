#ifndef __TERMINAL_X11_WINDOW_H
#define __TERMINAL_X11_WINDOW_H

#include <Yttrium/window.h>

#include "../../base/private_base.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Yttrium
{

class Y_PRIVATE Window::Private: public PrivateBase<Window::Private>
{
public:

	Screen _screen;

	::Display  *_display;
	int         _x_screen;
	::Window    _window;
	::Atom      _wm_protocols;
	::Atom      _wm_delete_window;
	GLXContext  _glx_context;

	Dim2               _size;
	Renderer::Private *_renderer; // See Renderer::Private constructor/destructor.

public:

	Private(const Screen &screen, ::Display *display, int x_screen, ::Window window, ::GLXContext glx_context, Allocator *allocator);

	~Private();

public:

	void close();

public:

	static bool create_window(::Display *display, int screen, ::Window *window, ::GLXContext *glx_context);

	static Key decode_key(::XEvent &event);
};

} // namespace Yttrium

#endif // __TERMINAL_X11_WINDOW_H
