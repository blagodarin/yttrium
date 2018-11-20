#pragma once

#include <yttrium/math/size.h>
#include "application.h"
#include "glx.h"

#include <optional>

namespace Yttrium
{
	class Point;
	class WindowBackendCallbacks;

	class WindowBackend
	{
	public:
		WindowBackend(const std::string& name, WindowBackendCallbacks&);
		~WindowBackend() noexcept;

		void close() noexcept;
		bool get_cursor(Point&);
		bool process_events();
		bool set_cursor(const Point&);
		void show();
		std::optional<Size> size() const { return _size; }
		void swap_buffers();

	private:
		class WindowHandle
		{
		public:
			WindowHandle(::Display* display, ::Window window)
				: _display(display), _window(window) {}
			~WindowHandle() noexcept { reset(); }
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
			~EmptyCursor() noexcept;
			::Cursor get() const { return _cursor; }

		private:
			::Display* const _display;
			::Cursor _cursor = None;
		};

		std::optional<Size> _size;
		NativeApplication _application;
		const GlxContext _glx{ _application.display(), _application.screen() };
		WindowHandle _window;
		const EmptyCursor _empty_cursor{ _application.display(), _window.get() };
		::Atom _wm_protocols = ::XInternAtom(_application.display(), "WM_PROTOCOLS", True);
		::Atom _wm_delete_window = ::XInternAtom(_application.display(), "WM_DELETE_WINDOW", True);
		::Atom _net_wm_state = ::XInternAtom(_application.display(), "_NET_WM_STATE", True);
		::Atom _net_wm_state_fullscreen = ::XInternAtom(_application.display(), "_NET_WM_STATE_FULLSCREEN", True);
		WindowBackendCallbacks& _callbacks;
	};
}
