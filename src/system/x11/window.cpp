#include "window.h"

#include <yttrium/exceptions.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include <yttrium/string.h>
#include "../../window/backend.h"

#include <cstring>

#include <X11/Xatom.h>

namespace
{
	using namespace Yttrium;

	P_Display open_display()
	{
		P_Display display(::XOpenDisplay(nullptr));
		if (!display)
			throw InitializationError("Failed to open the default X11 display");
		return display;
	}

	::Window create_window(::Display* display, int screen, const GlxContext& glx)
	{
		const auto root_window = RootWindow(display, screen);

		::XSetWindowAttributes swa;
		swa.colormap = ::XCreateColormap(display, root_window, glx.visual_info()->visual, AllocNone);
		swa.background_pixmap = None;
		swa.border_pixel = 0;
		swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | FocusChangeMask | StructureNotifyMask;

		const auto window = ::XCreateWindow(display, root_window,
			0, 0, 1, 1, 0, glx.visual_info()->depth, InputOutput, glx.visual_info()->visual,
			CWBorderPixel | CWColormap | CWEventMask, &swa);

		if (window == None)
		{
			::XFreeColormap(display, swa.colormap);
			throw InitializationError("Failed to create an X11 window");
		}

		return window;
	}

	Key key_from_event(::XEvent& event)
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
}

namespace Yttrium
{
	WindowBackend::WindowBackend(const String& name, WindowBackendCallbacks& callbacks)
		: _display(::open_display())
		, _window(_display.get(), ::create_window(_display.get(), _screen, _glx))
		, _callbacks(callbacks)
	{
		::XSetWMProtocols(_display.get(), _window.get(), &_wm_delete_window, 1);
		::XStoreName(_display.get(), _window.get(), name.text());

		// Hide system cursor.
		::XDefineCursor(_display.get(), _window.get(), _empty_cursor.get());

		// Show window in fullscreen mode.
		::XChangeProperty(_display.get(), _window.get(), _net_wm_state, XA_ATOM, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&_net_wm_state_fullscreen), 1);

		_glx.bind(_window.get());

		// Force vsync.
		if (_glx->EXT_swap_control)
			_glx->SwapIntervalEXT(_display.get(), _window.get(), _glx->EXT_swap_control_tear ? -1 : 1);
	}

	WindowBackend::~WindowBackend() = default;

	void WindowBackend::close()
	{
		if (!_window)
			return;
		_glx.unbind();
		_window.reset();
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		if (!_window)
			return false;

		::Window root = None;
		::Window child = None;
		int root_x = 0;
		int root_y = 0;
		int window_x = 0;
		int window_y = 0;
		unsigned mask = 0;

		if (!::XQueryPointer(_display.get(), _window.get(), &root, &child, &root_x, &root_y, &window_x, &window_y, &mask))
			return false;

		cursor = { window_x, window_y };

		return true;
	}

	bool WindowBackend::process_events()
	{
		if (!_window)
			return false;
		while (!_has_size ||::XPending(_display.get()) > 0)
		{
			::XEvent event;
			::XNextEvent(_display.get(), &event); // TODO: Don't process events for all windows.
			switch (event.type)
			{
			case KeyPress:
			case KeyRelease:
				_callbacks.on_key_event(::key_from_event(event), event.type == KeyPress);
				break;

			case ButtonPress:
			case ButtonRelease:
				switch (event.xbutton.button)
				{
				case Button1:
					_callbacks.on_key_event(Key::Mouse1, event.type == ButtonPress);
					break;

				case Button2:
					_callbacks.on_key_event(Key::Mouse2, event.type == ButtonPress);
					break;

				case Button3:
					_callbacks.on_key_event(Key::Mouse3, event.type == ButtonPress);
					break;

				case Button4:
					_callbacks.on_key_event(Key::Mouse4, event.type == ButtonPress);
					break;

				case Button5:
					_callbacks.on_key_event(Key::Mouse5, event.type == ButtonPress);
					break;
				}
				break;

			case FocusIn:
			case FocusOut:
				_callbacks.on_focus_event(event.type == FocusIn);
				break;

			case ConfigureNotify:
				_has_size = true;
				_callbacks.on_resize_event({ event.xconfigure.width, event.xconfigure.height });
				break;

			case ClientMessage:
				if (event.xclient.message_type == _wm_protocols
					&& event.xclient.data.l[0] == static_cast<long>(_wm_delete_window))
				{
					close();
					return false;
				}
				break;
			}
		}
		return true;
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		if (!_window)
			return false;
		::XWarpPointer(_display.get(), None, _window.get(), 0, 0, 0, 0, cursor.x(), cursor.y());
		::XSync(_display.get(), False);
		return true;
	}

	void WindowBackend::show()
	{
		if (_window)
			::XMapRaised(_display.get(), _window.get());
	}

	void WindowBackend::swap_buffers()
	{
		if (_window)
			_glx.swap_buffers(_window.get());
	}

	void WindowBackend::WindowHandle::reset() noexcept
	{
		if (_window == None)
			return;
		::XDestroyWindow(_display, _window);
		_window = None;
	}

	WindowBackend::EmptyCursor::EmptyCursor(::Display* display, ::Window window)
		: _display(display)
	{
		char data[1] = { 0 };
		const auto pixmap = ::XCreateBitmapFromData(_display, window, data, 1, 1);
		if (pixmap == None)
			throw InitializationError("Failed to create a pixmap for an empty cursor");
		::XColor color;
		::memset(&color, 0, sizeof(color));
		_cursor = ::XCreatePixmapCursor(_display, pixmap, pixmap, &color, &color, 0, 0);
		::XFreePixmap(_display, pixmap);
		if (_cursor == None)
			throw InitializationError("Failed to create an empty cursor");
	}

	WindowBackend::EmptyCursor::~EmptyCursor()
	{
		::XFreeCursor(_display, _cursor);
	}
}
