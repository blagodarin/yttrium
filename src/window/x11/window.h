#ifndef _src_window_x11_window_h_
#define _src_window_x11_window_h_

#include <yttrium/memory/unique_ptr.h>
#include "gl.h"

#include <X11/Xlib.h>

namespace Yttrium
{
	class Point;
	class ScreenImpl;
	class String;
	class WindowBackendCallbacks;

	class WindowBackend
	{
	public:
		WindowBackend(const ScreenImpl&, const String& name, WindowBackendCallbacks&, Allocator&);
		~WindowBackend();

		void close();
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void show();
		void swap_buffers();

	private:
		class EmptyCursor;

		::Display* const _display;
		const GlContext _glx;
		::Window _window; // TODO: Prevent resource leak if _empty_cursor construction throws.
		const UniquePtr<EmptyCursor> _empty_cursor;
		::Atom _wm_protocols = None;
		::Atom _wm_delete_window = None;
		::Atom _net_wm_state = None;
		::Atom _net_wm_state_fullscreen = None;
		WindowBackendCallbacks& _callbacks;
		bool _has_size = false;
	};
}

#endif
