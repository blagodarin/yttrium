#pragma once

#include <yttrium/math/size.h>
#include "../../utils/memory.h"

#include <optional>

#include <xcb/xcb.h>

namespace Yttrium
{
	class WindowBackendCallbacks;

	using P_Atom = UniquePtr<xcb_intern_atom_reply_t, std::free>;

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
		std::optional<Size> size() const noexcept { return _size; }
		void swap_buffers();

		xcb_connection_t* xcb_connection() const noexcept { return _connection.get(); }
		xcb_window_t xcb_window() const noexcept { return _window; }

	private:
		class EmptyCursor;
		class Keyboard;

		WindowBackendCallbacks& _callbacks;
		UniquePtr<xcb_connection_t, ::xcb_disconnect> _connection;
		std::unique_ptr<Keyboard> _keyboard;
		xcb_screen_t* _screen = nullptr;
		xcb_window_t _window = XCB_WINDOW_NONE;
		P_Atom _wm_protocols;
		P_Atom _wm_delete_window;
		std::unique_ptr<EmptyCursor> _empty_cursor;
		std::optional<Size> _size;
	};
}
