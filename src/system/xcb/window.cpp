#include "window.h"

#include "../../window/backend.h"

#include <string_view>

#include <xcb/xcb_image.h>

namespace
{
	using namespace Yttrium;

	auto make_atom(xcb_connection_t* connection, bool only_if_exists, std::string_view name)
	{
		return P_Atom{::xcb_intern_atom_reply(connection, ::xcb_intern_atom(connection, only_if_exists, static_cast<uint16_t>(name.size()), name.data()), nullptr)};
	}

	Key make_key(xcb_keycode_t keycode)
	{
		switch (keycode)
		{
		case 0x09: return Key::Escape;
		case 0x0a: return Key::_1;
		case 0x0b: return Key::_2;
		case 0x0c: return Key::_3;
		case 0x0d: return Key::_4;
		case 0x0e: return Key::_5;
		case 0x0f: return Key::_6;
		case 0x10: return Key::_7;
		case 0x11: return Key::_8;
		case 0x12: return Key::_9;
		case 0x13: return Key::_0;
		case 0x14: return Key::Minus;
		case 0x15: return Key::Equals;
		case 0x16: return Key::Backspace;
		case 0x17: return Key::Tab;
		case 0x18: return Key::Q;
		case 0x19: return Key::W;
		case 0x1a: return Key::E;
		case 0x1b: return Key::R;
		case 0x1c: return Key::T;
		case 0x1d: return Key::Y;
		case 0x1e: return Key::U;
		case 0x1f: return Key::I;
		case 0x20: return Key::O;
		case 0x21: return Key::P;
		case 0x22: return Key::LBracket;
		case 0x23: return Key::RBracket;
		case 0x24: return Key::Enter;
		case 0x25: return Key::LControl;
		case 0x26: return Key::A;
		case 0x27: return Key::S;
		case 0x28: return Key::D;
		case 0x29: return Key::F;
		case 0x2a: return Key::G;
		case 0x2b: return Key::H;
		case 0x2c: return Key::J;
		case 0x2d: return Key::K;
		case 0x2e: return Key::L;
		case 0x2f: return Key::Semicolon;
		case 0x30: return Key::Apostrophe;
		case 0x31: return Key::Grave;
		case 0x32: return Key::LShift;
		case 0x33: return Key::Backslash;
		case 0x34: return Key::Z;
		case 0x35: return Key::X;
		case 0x36: return Key::C;
		case 0x37: return Key::V;
		case 0x38: return Key::B;
		case 0x39: return Key::N;
		case 0x3a: return Key::M;
		case 0x3b: return Key::Comma;
		case 0x3c: return Key::Period;
		case 0x3d: return Key::Slash;
		case 0x3e: return Key::RShift;
		case 0x3f: return Key::Multiply;
		case 0x40: return Key::LAlt;
		case 0x41: return Key::Space;
		//case 0x42: return Key::CapsLock;
		case 0x43: return Key::F1;
		case 0x44: return Key::F2;
		case 0x45: return Key::F3;
		case 0x46: return Key::F4;
		case 0x47: return Key::F5;
		case 0x48: return Key::F6;
		case 0x49: return Key::F7;
		case 0x4a: return Key::F8;
		case 0x4b: return Key::F9;
		case 0x4c: return Key::F10;
		//case 0x4d: return Key::NumLock;
		//case 0x4e: return Key::ScrollLock;
		case 0x4f: return Key::Num7;
		case 0x50: return Key::Num8;
		case 0x51: return Key::Num9;
		case 0x52: return Key::Subtract;
		case 0x53: return Key::Num4;
		case 0x54: return Key::Num5;
		case 0x55: return Key::Num6;
		case 0x56: return Key::Add;
		case 0x57: return Key::Num1;
		case 0x58: return Key::Num2;
		case 0x59: return Key::Num3;
		case 0x5a: return Key::Num0;
		case 0x5b: return Key::Decimal;

		case 0x5e: return Key::Backslash; // TODO: Fix duplicate key.
		case 0x5f: return Key::F11;

		case 0x68: return Key::NumEnter;
		case 0x69: return Key::RControl;
		case 0x6a: return Key::Divide;

		case 0x6c: return Key::RAlt;

		case 0x6e: return Key::Home;
		case 0x6f: return Key::Up;
		case 0x70: return Key::PageUp;
		case 0x71: return Key::Left;
		case 0x72: return Key::Right;
		case 0x73: return Key::End;
		case 0x74: return Key::Down;
		case 0x75: return Key::PageDown;
		case 0x76: return Key::Insert;
		case 0x77: return Key::Delete;

		case 0x7f: return Key::Pause;

		//case 0x85: return Key::LWin;
		//case 0x86: return Key::RWin;
		//case 0x87: return Key::???; // TODO: What's the key between RWin and RControl?
		}
		return Key::Null;
	}

	Size make_size(const xcb_configure_notify_event_t& e)
	{
		return {e.width, e.height};
	}
}

namespace Yttrium
{
	class WindowBackend::EmptyCursor
	{
	public:
		xcb_connection_t* const _connection;
		const xcb_cursor_t _cursor;

		EmptyCursor(xcb_connection_t* connection, xcb_window_t window)
			: _connection{connection}
			, _cursor{::xcb_generate_id(_connection)}
		{
			uint8_t data = 0;
			const auto pixmap = ::xcb_create_pixmap_from_bitmap_data(_connection, window, &data, 1, 1, 1, 0, 0, nullptr);
			::xcb_create_cursor(_connection, _cursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
			::xcb_free_pixmap(_connection, pixmap);
		}

		~EmptyCursor()
		{
			::xcb_free_cursor(_connection, _cursor);
		}
	};

	WindowBackend::WindowBackend(const std::string& name, WindowBackendCallbacks& callbacks)
		: _callbacks{callbacks}
	{
		int preferred_screen = 0;
		_connection.reset(::xcb_connect(nullptr, &preferred_screen));
		if (::xcb_connection_has_error(_connection.get()))
			return;

		auto screen_iterator = ::xcb_setup_roots_iterator(::xcb_get_setup(_connection.get()));
		for (; preferred_screen > 0; --preferred_screen)
			::xcb_screen_next(&screen_iterator);
		_screen = screen_iterator.data;

		_window = ::xcb_generate_id(_connection.get());

		const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		const uint32_t list[] =
		{
			_screen->black_pixel,
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_FOCUS_CHANGE,
		};
		::xcb_create_window(_connection.get(), XCB_COPY_FROM_PARENT, _window, _screen->root,
			0, 0, _screen->width_in_pixels, _screen->height_in_pixels, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, _screen->root_visual, mask, list);

		_wm_protocols = ::make_atom(_connection.get(), true, "WM_PROTOCOLS");
		_wm_delete_window = ::make_atom(_connection.get(), false, "WM_DELETE_WINDOW");
		::xcb_change_property(_connection.get(), XCB_PROP_MODE_REPLACE, _window, _wm_protocols->atom, XCB_ATOM_ATOM, 32, 1, &_wm_delete_window->atom);

		::xcb_change_property(_connection.get(), XCB_PROP_MODE_REPLACE, _window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, static_cast<uint32_t>(name.size()), name.data());

		_empty_cursor = std::make_unique<EmptyCursor>(_connection.get(), _window);
		::xcb_change_window_attributes(_connection.get(), _window, XCB_CW_CURSOR, &_empty_cursor->_cursor);

		{
			const auto net_wm_state = ::make_atom(_connection.get(), false, "_NET_WM_STATE");
			const auto net_wm_state_fullscreen = ::make_atom(_connection.get(), false, "_NET_WM_STATE_FULLSCREEN");
			::xcb_change_property(_connection.get(), XCB_PROP_MODE_REPLACE, _window, net_wm_state->atom, XCB_ATOM_ATOM, 32, 1, &net_wm_state_fullscreen->atom);
		}

		::xcb_flush(_connection.get());
	}

	WindowBackend::~WindowBackend()
	{
		close();
	}

	void WindowBackend::close()
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		::xcb_destroy_window(_connection.get(), _window);
		::xcb_flush(_connection.get());
		_window = XCB_WINDOW_NONE;
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		if (_window == XCB_WINDOW_NONE)
			return false;
		const Y_UNIQUE_PTR(xcb_query_pointer_reply_t, std::free) reply{::xcb_query_pointer_reply(_connection.get(), ::xcb_query_pointer(_connection.get(), _window), nullptr)};
		if (!reply)
			return false;
		cursor = {reply->win_x, reply->win_y};
		return true;
	}

	bool WindowBackend::process_events()
	{
		if (_window == XCB_WINDOW_NONE)
			return false;

		for (;;)
		{
			const Y_UNIQUE_PTR(xcb_generic_event_t, std::free) event{_size ? ::xcb_poll_for_event(_connection.get()) : ::xcb_wait_for_event(_connection.get())};
			if (!event)
				return !::xcb_connection_has_error(_connection.get());

			switch (const auto event_type = event->response_type & 0x7f)
			{
			case XCB_KEY_PRESS:
			case XCB_KEY_RELEASE:
				// TODO: Use modifiers from XCB.
				_callbacks.on_key_event(::make_key(reinterpret_cast<xcb_key_press_event_t*>(event.get())->detail), event_type == XCB_KEY_PRESS);
				break;

			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
				switch (const auto e = reinterpret_cast<xcb_button_press_event_t*>(event.get()); e->detail)
				{
				case XCB_BUTTON_INDEX_1:
					_callbacks.on_key_event(Key::Mouse1, event_type == XCB_BUTTON_PRESS);
					break;

				case XCB_BUTTON_INDEX_2:
					_callbacks.on_key_event(Key::Mouse2, event_type == XCB_BUTTON_PRESS);
					break;

				case XCB_BUTTON_INDEX_3:
					_callbacks.on_key_event(Key::Mouse3, event_type == XCB_BUTTON_PRESS);
					break;

				case XCB_BUTTON_INDEX_4:
					_callbacks.on_key_event(Key::Mouse4, event_type == XCB_BUTTON_PRESS);
					break;

				case XCB_BUTTON_INDEX_5:
					_callbacks.on_key_event(Key::Mouse5, event_type == XCB_BUTTON_PRESS);
					break;
				}
				break;

			case XCB_FOCUS_IN:
			case XCB_FOCUS_OUT:
				_callbacks.on_focus_event(event_type == XCB_FOCUS_IN);
				break;

			case XCB_CONFIGURE_NOTIFY:
				_size = ::make_size(*reinterpret_cast<xcb_configure_notify_event_t*>(event.get()));
				_callbacks.on_resize_event(*_size);
				break;

			case XCB_CLIENT_MESSAGE:
				if (const auto e = reinterpret_cast<xcb_client_message_event_t*>(event.get());
					e->type == _wm_protocols->atom && e->data.data32[0] == _wm_delete_window->atom)
				{
					close();
					return false;
				}
				break;
			}
		}
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		if (_window == XCB_WINDOW_NONE)
			return false;
		::xcb_warp_pointer(_connection.get(), XCB_WINDOW_NONE, _window, 0, 0, 0, 0, static_cast<int16_t>(cursor._x), static_cast<int16_t>(cursor._y));
		::xcb_flush(_connection.get());
		return true;
	}

	void WindowBackend::show()
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		::xcb_map_window(_connection.get(), _window);
		::xcb_flush(_connection.get());
	}

	void WindowBackend::swap_buffers()
	{
		// TODO: Implement.
	}
}
