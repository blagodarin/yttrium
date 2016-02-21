#ifndef _src_window_x11_window_h_
#define _src_window_x11_window_h_

#include <yttrium/key.h>
#include <yttrium/pointer.h>
#include "../backend.h"

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Yttrium
{
	class Point;
	class Rect;
	class ScreenImpl;
	class Size;
	class StaticString;

	class WindowBackend
	{
	public:

		static Pointer<WindowBackend> create(Allocator& allocator, const ScreenImpl& screen, WindowBackendCallbacks&);

		WindowBackend(Allocator& allocator, ::Display* display, ::Window window, ::GLXContext glx_context, WindowBackendCallbacks&);
		~WindowBackend();

		void close();
		bool get_cursor(Point& cursor);
		bool process_events();
		bool set_cursor(const Point& cursor);
		void set_name(const StaticString& name);
		void show();
		void show_cursor(bool);
		void swap_buffers();

	private:
		class EmptyCursor;

		::Display* _display;
		::Window _window;
		::Atom _wm_protocols = None;
		::Atom _wm_delete_window = None;
		::Atom _net_wm_state = None;
		::Atom _net_wm_state_fullscreen = None;
		const Pointer<EmptyCursor> _empty_cursor;
		::GLXContext _glx_context;
		WindowBackendCallbacks& _callbacks;
		bool _has_size = false;
	};
}

#endif
