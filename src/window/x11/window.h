#ifndef __WINDOW_X11_BACKEND_H
#define __WINDOW_X11_BACKEND_H

#include <yttrium/key.h>
#include <yttrium/size.h>
#include <yttrium/vector.h>

#include <memory>

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Yttrium
{
	class Point;
	class ScreenImpl;
	class StaticString;

	class WindowBackend
	{
	public:

		class Callbacks
		{
		public:

			virtual void on_focus_event(bool is_focused) = 0;
			virtual void on_key_event(Key key, bool is_pressed) = 0;
		};

	public:

		WindowBackend(::Display* display, ::Window window, ::GLXContext glx_context, const Size& size, Callbacks& callbacks);
		~WindowBackend();

	public:

		void close();
		bool get_cursor(Point& cursor);
		bool get_frame_sync(bool& frame_sync);
		bool put(int left, int top, int width, int height, bool border);
		bool process_events();
		bool set_cursor(const Point& cursor);
		bool set_frame_sync(bool frame_sync);
		void set_name(const StaticString& name);
		void show();
		void swap_buffers();

	public:

		static std::unique_ptr<WindowBackend> create(const ScreenImpl& screen, const Size& size, Callbacks& callbacks);

	private:

		::Display*   _display;
		::Window     _window;
		::Atom       _wm_protocols;
		::Atom       _wm_delete_window;
		::GLXContext _glx_context;
		Size         _size;
		Callbacks&   _callbacks;
	};
}

#endif // __WINDOW_X11_BACKEND_H
