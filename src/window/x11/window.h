#ifndef _src_window_x11_window_h_
#define _src_window_x11_window_h_

#include <yttrium/key.h>
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
	template <typename> class Pointer;

	class WindowBackend
	{
	public:

		// TODO: Proper constructor.
		static Pointer<WindowBackend> create(Allocator& allocator, const ScreenImpl& screen, const Size& size, WindowBackendCallbacks&);

		WindowBackend(::Display* display, ::Window window, ::GLXContext glx_context, const Size& size, WindowBackendCallbacks&);
		~WindowBackend();

		void close();
		bool get_cursor(Point& cursor);
		bool get_frame_sync(bool& frame_sync);
		bool put(const Rect&, bool border);
		bool process_events();
		bool set_cursor(const Point& cursor);
		bool set_frame_sync(bool frame_sync);
		void set_name(const StaticString& name);
		void show();
		void swap_buffers();

	private:
		::Display* _display;
		::Window _window;
		::Atom _wm_protocols;
		::Atom _wm_delete_window;
		::GLXContext _glx_context;
		WindowBackendCallbacks& _callbacks;
	};
}

#endif
