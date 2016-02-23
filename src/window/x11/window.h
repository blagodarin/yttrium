#ifndef _src_window_x11_window_h_
#define _src_window_x11_window_h_

#include <yttrium/pointer.h>
#include "gl.h"

#include <X11/Xlib.h>

namespace Yttrium
{
	class Point;
	class StaticString;
	class WindowBackendCallbacks;

	class WindowBackend
	{
	public:

		static Pointer<WindowBackend> create(Allocator&, ::Display*, int screen, WindowBackendCallbacks&);

		WindowBackend(Allocator&, ::Display*, GlContext&&, ::Window, WindowBackendCallbacks&);
		~WindowBackend();

		void close();
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void set_name(const StaticString&);
		void show();
		void swap_buffers();

	private:
		class EmptyCursor;

		::Display* const _display;
		const GlContext _glx;
		::Window _window;
		const Pointer<EmptyCursor> _empty_cursor;
		::Atom _wm_protocols = None;
		::Atom _wm_delete_window = None;
		::Atom _net_wm_state = None;
		::Atom _net_wm_state_fullscreen = None;
		WindowBackendCallbacks& _callbacks;
		bool _has_size = false;
	};
}

#endif
