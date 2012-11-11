#include "window.h"

#include "../../renderer/renderer.h"

#include "screen.h"

namespace Yttrium
{

Window::Private::Private(const Screen &screen, ::Display *display, int x_screen, ::Window window, ::GLXContext glx_context, Allocator *allocator)
	: PrivateBase(allocator)
	, _screen(screen)
	, _display(display)
	, _x_screen(x_screen)
	, _window(window)
	, _wm_protocols(XInternAtom(display, "WM_PROTOCOLS", True))
	, _wm_delete_window(XInternAtom(display, "WM_DELETE_WINDOW", True))
	, _glx_context(glx_context)
	, _renderer(nullptr)
{
	::XSetWMProtocols(display, window, &_wm_delete_window, 1);
	set_fixed_size(320, 240); // NOTE: Magic default.
}

Window::Private::~Private()
{
	close();
	::glXDestroyContext(_display, _glx_context);
}

void Window::Private::close()
{
	if (_window != None)
	{
		::glXMakeCurrent(_display, None, nullptr);
		::XDestroyWindow(_display, _window);
		_window = None;
	}
}

void Window::Private::set_fixed_size(int width, int height)
{
	::XSizeHints size_hints;

	size_hints.min_width = width;
	size_hints.min_height = height;
	size_hints.max_width = width;
	size_hints.max_height = height;
	size_hints.flags = PMinSize | PMaxSize;

	::XSetWMNormalHints(_display, _window, &size_hints);
}

bool Window::Private::create_window(::Display *display, int screen, ::Window *window, ::GLXContext *glx_context)
{
	const int attributes[] =
	{
		GLX_CONFIG_CAVEAT, GLX_NONE,
		GLX_X_RENDERABLE,  True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, // Default.
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,   // Default.
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE,      8,
		GLX_GREEN_SIZE,    8,
		GLX_BLUE_SIZE,     8,
		GLX_ALPHA_SIZE,    8,
		GLX_DEPTH_SIZE,    24,
		GLX_STENCIL_SIZE,  8,
		GLX_DOUBLEBUFFER,  True,
		None
	};

	int fb_count = 0;

	::GLXFBConfig *fb_config = ::glXChooseFBConfig(display, screen, attributes, &fb_count);

	::XVisualInfo *vi = nullptr;

	if (fb_config)
	{
		if (fb_count)
		{
			int best_config = 0; // TODO: Improve.
			vi = ::glXGetVisualFromFBConfig(display, fb_config[best_config]);
		}
		::XFree(fb_config);
	}

	if (vi)
	{
		screen = vi->screen; // NOTE: Why?

		*glx_context = ::glXCreateContext(display, vi, nullptr, True);

		if (*glx_context)
		{
			if (::glXIsDirect(display, *glx_context))
			{
				::XSetWindowAttributes swa;

				swa.colormap = ::XCreateColormap(display, RootWindow(display, screen), vi->visual, AllocNone);
				swa.background_pixmap = None;
				swa.border_pixel = 0;
				swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | FocusChangeMask;

				*window = ::XCreateWindow(display, RootWindow(display, screen),
					0, 0, 320, 240, 0, vi->depth, InputOutput, vi->visual, // NOTE: Magic default.
					CWBorderPixel | CWColormap | CWEventMask, &swa);

				if (*window != None)
				{
					::glXMakeCurrent(display, *window, *glx_context);

					return true;
				}
				else
				{
					::XFreeColormap(display, swa.colormap);
				}
			}

			::glXDestroyContext(display, *glx_context);
			*glx_context = nullptr;
		}
	}

	return false;
}

Key Window::Private::decode_key(::XEvent &event)
{
	::KeySym key_sym = ::XLookupKeysym(&event.xkey, 0);

	if (key_sym >= XK_0 && key_sym <= XK_9)
	{
		return static_cast<Key>(KeyType(Key::_0) + key_sym - XK_0);
	}
	else if (key_sym >= XK_a && key_sym <= XK_z)
	{
		return static_cast<Key>(KeyType(Key::A) + key_sym - XK_a);
	}
	else if (key_sym >= XK_F1 && key_sym <= XK_F24)
	{
		return static_cast<Key>(KeyType(Key::F1) + key_sym - XK_F1);
	}
	else if (key_sym >= XK_KP_0 && key_sym <= XK_KP_9)
	{
		return static_cast<Key>(KeyType(Key::Num0) + key_sym - XK_KP_0);
	}
	else switch (key_sym)
	{
	case XK_space:        return Key::Space;      // 0x0020

	case XK_apostrophe:   return Key::Apostrophe; // 0x0027

	case XK_comma:        return Key::Comma;      // 0x002C
	case XK_minus:        return Key::Minus;
	case XK_period:       return Key::Period;
	case XK_slash:        return Key::Slash;

	case XK_semicolon:    return Key::Semicolon;  // 0x003B

	case XK_equal:        return Key::Equals;     // 0x003D

	case XK_bracketleft:  return Key::LBracket;   // 0x005B
	case XK_backslash:    return Key::Backslash;
	case XK_bracketright: return Key::RBracket;

	case XK_grave:        return Key::Grave;      // 0x0060

	case XK_BackSpace:    return Key::Backspace;  // 0xFF08
	case XK_Tab:          return Key::Tab;
	case XK_Return:       return Key::Enter;
	case XK_Pause:        return Key::Pause;
	case XK_Sys_Req:      return Key::Print;
	case XK_Escape:       return Key::Escape;

	case XK_Home:         return Key::Home;       // 0xFF50
	case XK_Left:         return Key::Left;
	case XK_Up:           return Key::Up;
	case XK_Right:        return Key::Right;
	case XK_Down:         return Key::Down;
	case XK_Prior:        return Key::PageUp;
	case XK_Next:         return Key::PageDown;
	case XK_End:          return Key::End;

	case XK_Insert:       return Key::Insert;     // 0xFF63

	case XK_KP_Enter:     return Key::NumEnter;   // 0xFF8D

	case XK_KP_Home:      return Key::Num7;       // 0xFF95
	case XK_KP_Left:      return Key::Num4;
	case XK_KP_Up:        return Key::Num8;
	case XK_KP_Right:     return Key::Num6;
	case XK_KP_Down:      return Key::Num2;
	case XK_KP_Prior:     return Key::Num9;
	case XK_KP_Next:      return Key::Num3;
	case XK_KP_End:       return Key::Num1;

	case XK_KP_Multiply:  return Key::Multiply;   // 0xFFAA
	case XK_KP_Add:       return Key::Add;

	case XK_KP_Subtract:  return Key::Subtract;   // 0xFFAD
	case XK_KP_Decimal:   return Key::Decimal;
	case XK_KP_Divide:    return Key::Divide;

	case XK_Shift_L:      return Key::LShift;     // 0xFFE1
	case XK_Shift_R:      return Key::RShift;
	case XK_Control_L:    return Key::LControl;
	case XK_Control_R:    return Key::RControl;

	case XK_Alt_L:        return Key::LAlt;       // 0xFFE9
	case XK_Alt_R:        return Key::RAlt;

	case XK_Delete:       return Key::Delete;     // 0xFFFF
	}

	return Key::Null;
}

bool Window::get_cursor(Dim2 *cursor)
{
	if (!_private || _private->_window == None)
	{
		return false;
	}

	// NOTE: Don't know if I need all of these.

	::Window root_return, child_return;
	int root_x_return, root_y_return;
	int win_x_return, win_y_return;
	unsigned int mask_return;

	if (!::XQueryPointer(_private->_display, _private->_window,
		&root_return, &child_return, &root_x_return, &root_y_return,
		&win_x_return, &win_y_return, &mask_return))
	{
		return false;
	}

	cursor->x = win_x_return;
	cursor->y = win_y_return;

	return true;
}

bool Window::get_frame_sync(bool *frame_sync)
{
	Y_UNUSED(frame_sync);

	return false;
}

bool Window::open(const Screen &screen, Callbacks *callbacks, Allocator *allocator)
{
	close();

	if (screen.is_opened())
	{
		::Display    *display  = screen._private->_display;
		int           x_screen = screen._private->_screen;
		::Window      window;
		::GLXContext  glx_context;

		if (Private::create_window(display, x_screen, &window, &glx_context))
		{
			_private = Y_NEW(allocator, Private)(screen, display, x_screen, window, glx_context, allocator);
			_callbacks = callbacks;
			return true;
		}
	}

	return false;
}

bool Window::put(Dim left, Dim top, Dim width, Dim height, PutMode mode)
{
	if (_private->_window == None)
	{
		return false;
	}

	::XSetWindowAttributes attributes;

	attributes.override_redirect = (mode == NoBorder ? True : False);
	::XChangeWindowAttributes(_private->_display, _private->_window, CWOverrideRedirect, &attributes);
	::XMoveResizeWindow(_private->_display, _private->_window, left, top, width, height);
	_private->set_fixed_size(width, height);

	_private->_size = Dim2(width, height);

	if (_private->_renderer)
	{
		_private->_renderer->set_viewport(_private->_size);
	}

	return true;
}

bool Window::process_events()
{
	if (!_private || _private->_window == None)
	{
		return false;
	}

	while (::XPending(_private->_display))
	{
		::XEvent event;

		::XNextEvent(_private->_display, &event);
		if (event.type == ClientMessage)
		{
			if (event.xclient.message_type == _private->_wm_protocols
				&& ::Atom(event.xclient.data.l[0]) == _private->_wm_delete_window)
			{
				_private->close();
				return false;
			}
		}
		else if (_callbacks)
		{
			switch (event.type)
			{
			case KeyPress:

				_callbacks->on_key_event(this, Private::decode_key(event), true);
				break;

			case KeyRelease:

				_callbacks->on_key_event(this, Private::decode_key(event), false);
				break;

			case ButtonPress:

				if (event.xbutton.button == Button1)
				{
					_callbacks->on_key_event(this, Key::Mouse1, true);
				}
				else if (event.xbutton.button == Button2)
				{
					_callbacks->on_key_event(this, Key::Mouse2, true);
				}
				else if (event.xbutton.button == Button3)
				{
					_callbacks->on_key_event(this, Key::Mouse3, true);
				}
				else if (event.xbutton.button == Button4)
				{
					_callbacks->on_key_event(this, Key::Mouse4, true);
				}
				else if (event.xbutton.button == Button5)
				{
					_callbacks->on_key_event(this, Key::Mouse5, true);
				}
				break;

			case ButtonRelease:

				if (event.xbutton.button == Button1)
				{
					_callbacks->on_key_event(this, Key::Mouse1, false);
				}
				else if (event.xbutton.button == Button2)
				{
					_callbacks->on_key_event(this, Key::Mouse2, false);
				}
				else if (event.xbutton.button == Button3)
				{
					_callbacks->on_key_event(this, Key::Mouse3, false);
				}
				else if (event.xbutton.button == Button4)
				{
					_callbacks->on_key_event(this, Key::Mouse4, false);
				}
				else if (event.xbutton.button == Button5)
				{
					_callbacks->on_key_event(this, Key::Mouse5, false);
				}
				break;

			case FocusIn:

				_callbacks->on_focus_event(this, true);
				break;

			case FocusOut:

				_callbacks->on_focus_event(this, false);
				break;
			}
		}
	}

	return true;
}

void Window::set_name(const StaticString &name)
{
	if (_private && _private->_window != None)
	{
		::XStoreName(_private->_display, _private->_window, name.text());
	}
}

bool Window::set_cursor(const Dim2 &cursor)
{
	if (!_private || _private->_window == None)
	{
		return false;
	}

	::XWarpPointer(_private->_display, None, _private->_window, 0, 0, 0, 0, cursor.x, cursor.y);
	return true;
}

bool Window::set_frame_sync(bool frame_sync)
{
	Y_UNUSED(frame_sync);

	return false;
}

void Window::show(ShowMode mode)
{
	if (_private->_window == None)
	{
		return;
	}

	switch (mode)
	{
	case Show:

		::XMapWindow(_private->_display, _private->_window);
		break;

	case Focus:

		::XMapRaised(_private->_display, _private->_window);
		//::XSetInputFocus(_private->_display, _private->_window, RevertToNone, CurrentTime);
		break;

	default:

		::XUnmapWindow(_private->_display, _private->_window);
		break;
	}
}

void Window::swap_buffers()
{
	if (_private && _private->_window != None)
	{
		::glXSwapBuffers(_private->_display, _private->_window);
	}
}

void Window::terminate()
{
	if (_private)
	{
		_private->close();
	}
}

} // namespace Yttrium
