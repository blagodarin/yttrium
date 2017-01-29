#ifndef _src_system_x11_window_h_
#define _src_system_x11_window_h_

#include <yttrium/math/size.h>
#include <yttrium/memory/unique_ptr.h>
#include "../../utils/unique_ptr.h"
#include "glx.h"

#include <boost/optional/optional.hpp>

#include <X11/Xlib.h>

namespace Yttrium
{
	class Point;
	class String;
	class WindowBackendCallbacks;

	using P_Display = Y_UNIQUE_PTR(::Display, ::XCloseDisplay);

	class WindowBackend
	{
	public:
		WindowBackend(const String& name, WindowBackendCallbacks&);
		~WindowBackend();

		void close();
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void show();
		boost::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		class WindowHandle
		{
		public:
			WindowHandle(::Display* display, ::Window window) : _display(display), _window(window) {}
			~WindowHandle() { reset(); }
			explicit operator bool() const noexcept { return _window != None; }
			::Window get() const noexcept { return _window; }
			void reset() noexcept;
		private:
			::Display* const _display;
			::Window _window = None;
		};

		class EmptyCursor
		{
		public:
			EmptyCursor(::Display*, ::Window);
			~EmptyCursor();
			::Cursor get() const { return _cursor; }
		private:
			::Display* const _display;
			::Cursor _cursor = None;
		};

		boost::optional<Size> _size;
		const P_Display _display;
		const int _screen = DefaultScreen(_display.get());
		const GlxContext _glx{ _display.get(), _screen };
		WindowHandle _window;
		const EmptyCursor _empty_cursor{ _display.get(), _window.get() };
		::Atom _wm_protocols = ::XInternAtom(_display.get(), "WM_PROTOCOLS", True);
		::Atom _wm_delete_window = ::XInternAtom(_display.get(), "WM_DELETE_WINDOW", True);
		::Atom _net_wm_state = ::XInternAtom(_display.get(), "_NET_WM_STATE", True);
		::Atom _net_wm_state_fullscreen = ::XInternAtom(_display.get(), "_NET_WM_STATE_FULLSCREEN", True);
		WindowBackendCallbacks& _callbacks;
	};
}

#endif
