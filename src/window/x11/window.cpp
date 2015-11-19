#include "window.h"

#include <yttrium/math/rect.h>
#include <yttrium/pointer.h>
#include <yttrium/static_string.h>
#include "../../renderer/gl/version.h"
#include "../gl.h"
#include "screen.h"

#include <cstring>

namespace Yttrium
{
	namespace
	{
		void fix_window_size(::Display* display, ::Window window, const Size& size)
		{
			::XSizeHints size_hints;
			::memset(&size_hints, 0, sizeof size_hints);
			size_hints.min_width = size.width();
			size_hints.min_height = size.height();
			size_hints.max_width = size.width();
			size_hints.max_height = size.height();
			size_hints.flags = PMinSize | PMaxSize;
			::XSetWMNormalHints(display, window, &size_hints);
		}

		// X error handling (see below).
		bool error_occurred = false;
		int error_handler(::Display*, ::XErrorEvent*)
		{
			error_occurred = true;
			return 0;
		}

		bool initialize_window(::Display* display, int screen, const Size& size, ::Window& window, ::GLXContext& glx_context)
		{
			// GLXFBConfig API requires GLX 1.3.
			// glXGetProcAddress, GLX_SAMPLE_BUFFERS and GLX_SAMPLES require GLX 1.4.
			int glx_version_major = 0;
			int glx_version_minor = 0;
			if (!::glXQueryVersion(display, &glx_version_major, &glx_version_minor)
				|| glx_version_major != 1 // GLX 2.0 may be completely different from GLX 1.x.
				|| glx_version_minor < 4)
				return false;

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

			Y_UNIQUE_PTR(::XVisualInfo, ::XFree) best_vi;
			::GLXFBConfig best_fbc = {};
			{
				int fbc_count = 0;
				const Y_UNIQUE_PTR(::GLXFBConfig[], ::XFree) fbcs(::glXChooseFBConfig(display, screen, attributes, &fbc_count));
				if (!fbcs)
					return false;

				for (int i = 0; i < fbc_count; ++i)
				{
					// The official OpenGL example suggest sorting by GLX_SAMPLE_BUFFERS
					// and GLX_SAMPLES, but we have no successful experience in using it.
					Y_UNIQUE_PTR(::XVisualInfo, ::XFree) vi(::glXGetVisualFromFBConfig(display, fbcs[i]));
					if (vi->depth == 24) // A depth of 32 will give us an ugly result.
					{
						best_vi = std::move(vi);
						best_fbc = fbcs[i];
						break;
					}
				}

				if (!best_vi)
					return false;
			}

			const auto check_extension = [](const char* list, const char* name)
			{
				const size_t name_size = ::strlen(name);
				while ((list = ::strstr(list, name)))
				{
					list += name_size;
					if (*list == ' ' || *list == 0)
						return true;
				}
				return false;
			};

			const auto glx_extensions = ::glXQueryExtensionsString(display, screen);
			if (!check_extension(glx_extensions, "GLX_ARB_create_context"))
				return false;

			using glXCreateContextAttribsARB_t = ::GLXContext (*)(::Display*, ::GLXFBConfig, ::GLXContext, Bool, const int*);
			const auto glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARB_t>(::glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));
			if (!glXCreateContextAttribsARB)
				return false;

			const int context_attributes[] =
			{
				GLX_CONTEXT_MAJOR_VERSION_ARB, Gl::required_major,
				GLX_CONTEXT_MINOR_VERSION_ARB, Gl::required_minor,
		#if Y_IS_DEBUG
				GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
		#endif
				None
			};

			// The actual context creation is wrapped in error handling code as advised
			// by the official OpenGL context creation tutorial. The tutorial also warns
			// that X error handling is global and not thread-safe.
			error_occurred = false;
			const auto old_error_handler = ::XSetErrorHandler(error_handler);
			glx_context = glXCreateContextAttribsARB(display, best_fbc, nullptr, True, context_attributes);
			::XSync(display, False); // To ensure any errors generated are processed.
			::XSetErrorHandler(old_error_handler);
			if (error_occurred || !glx_context)
				return false;

			if (::glXIsDirect(display, glx_context))
			{
				::Window root_window = RootWindow(display, screen);

				::XSetWindowAttributes swa;

				swa.colormap = ::XCreateColormap(display, root_window, best_vi->visual, AllocNone);
				swa.background_pixmap = None;
				swa.border_pixel = 0;
				swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | FocusChangeMask;

				window = ::XCreateWindow(display, root_window,
					0, 0, size.width(), size.height(), 0, best_vi->depth, InputOutput, best_vi->visual,
					CWBorderPixel | CWColormap | CWEventMask, &swa);

				if (window != None)
				{
					::glXMakeCurrent(display, window, glx_context);
					return true;
				}

				::XFreeColormap(display, swa.colormap);
			}

			::glXDestroyContext(display, glx_context);
			glx_context = nullptr;

			return false;
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

	Pointer<WindowBackend> WindowBackend::create(Allocator& allocator, const ScreenImpl& screen, const Size& size, WindowBackendCallbacks& callbacks)
	{
		::Window window_handle;
		::GLXContext glx_context;
		if (!initialize_window(screen.display(), screen.screen(), size, window_handle, glx_context))
			return {};
		return make_pointer<WindowBackend>(allocator, screen.display(), window_handle, glx_context, size, callbacks);
	}

	WindowBackend::WindowBackend(::Display* display, ::Window window, ::GLXContext glx_context, const Size& size, WindowBackendCallbacks& callbacks)
		: _display(display)
		, _window(window)
		, _wm_protocols(::XInternAtom(_display, "WM_PROTOCOLS", True))
		, _wm_delete_window(::XInternAtom(_display, "WM_DELETE_WINDOW", True))
		, _glx_context(glx_context)
		, _callbacks(callbacks)
	{
		::XSetWMProtocols(_display, _window, &_wm_delete_window, 1);
		fix_window_size(_display, _window, size);
	}

	WindowBackend::~WindowBackend()
	{
		close();
		::glXDestroyContext(_display, _glx_context);
	}

	void WindowBackend::close()
	{
		if (_window != None)
		{
			::glXMakeCurrent(_display, None, nullptr);
			::XDestroyWindow(_display, _window);
			_window = None;
		}
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		if (_window == None)
			return false;

		::Window root = None;
		::Window child = None;
		int root_x = 0;
		int root_y = 0;
		int window_x = 0;
		int window_y = 0;
		unsigned mask = 0;

		if (!::XQueryPointer(_display, _window, &root, &child, &root_x, &root_y, &window_x, &window_y, &mask))
			return false;

		cursor = {window_x, window_y};

		return true;
	}

	bool WindowBackend::get_frame_sync(bool& frame_sync)
	{
		Y_UNUSED(frame_sync);
		return false;
	}

	bool WindowBackend::put(const Rect& rect, bool border)
	{
		if (_window == None)
			return false;

		::XSetWindowAttributes attributes;
		attributes.override_redirect = border ? False : True;

		::XChangeWindowAttributes(_display, _window, CWOverrideRedirect, &attributes);
		::XMoveResizeWindow(_display, _window, rect.left(), rect.top(), rect.width(), rect.height());

		fix_window_size(_display, _window, rect.size());

		return true;
	}

	bool WindowBackend::process_events()
	{
		if (_window == None)
			return false;

		while (::XPending(_display))
		{
			::XEvent event;

			::XNextEvent(_display, &event);
			if (event.type == ClientMessage)
			{
				if (event.xclient.message_type == _wm_protocols && ::Atom(event.xclient.data.l[0]) == _wm_delete_window)
				{
					close();
					return false;
				}
			}
			else
			{
				switch (event.type)
				{
				case KeyPress:

					_callbacks.on_key_event(key_from_event(event), true);
					break;

				case KeyRelease:

					_callbacks.on_key_event(key_from_event(event), false);
					break;

				case ButtonPress:

					if (event.xbutton.button == Button1)
					{
						_callbacks.on_key_event(Key::Mouse1, true);
					}
					else if (event.xbutton.button == Button2)
					{
						_callbacks.on_key_event(Key::Mouse2, true);
					}
					else if (event.xbutton.button == Button3)
					{
						_callbacks.on_key_event(Key::Mouse3, true);
					}
					else if (event.xbutton.button == Button4)
					{
						_callbacks.on_key_event(Key::Mouse4, true);
					}
					else if (event.xbutton.button == Button5)
					{
						_callbacks.on_key_event(Key::Mouse5, true);
					}
					break;

				case ButtonRelease:

					if (event.xbutton.button == Button1)
					{
						_callbacks.on_key_event(Key::Mouse1, false);
					}
					else if (event.xbutton.button == Button2)
					{
						_callbacks.on_key_event(Key::Mouse2, false);
					}
					else if (event.xbutton.button == Button3)
					{
						_callbacks.on_key_event(Key::Mouse3, false);
					}
					else if (event.xbutton.button == Button4)
					{
						_callbacks.on_key_event(Key::Mouse4, false);
					}
					else if (event.xbutton.button == Button5)
					{
						_callbacks.on_key_event(Key::Mouse5, false);
					}
					break;

				case FocusIn:

					_callbacks.on_focus_event(true);
					break;

				case FocusOut:

					_callbacks.on_focus_event(false);
					break;
				}
			}
		}

		return true;
	}

	void WindowBackend::set_name(const StaticString& name)
	{
		if (_window == None)
			return;
		::XStoreName(_display, _window, name.text());
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		if (_window == None)
			return false;
		::XWarpPointer(_display, None, _window, 0, 0, 0, 0, cursor.x(), cursor.y());
		return true;
	}

	bool WindowBackend::set_frame_sync(bool frame_sync)
	{
		Y_UNUSED(frame_sync);
		return false;
	}

	void WindowBackend::show()
	{
		if (_window == None)
			return;
		::XMapRaised(_display, _window);
	}

	void WindowBackend::swap_buffers()
	{
		if (_window == None)
			return;
		::glXSwapBuffers(_display, _window);
	}
}
