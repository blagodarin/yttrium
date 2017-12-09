#include "window.h"

#include "../../config.h"
#include "../../window/backend.h"

#include <algorithm>
#include <string>

#include <xcb/xcb_image.h>
#define explicit explicit_ // TODO: Report an XKB bug.
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>

namespace
{
	using namespace Yttrium;

	auto make_atom(xcb_connection_t* connection, bool only_if_exists, std::string_view name)
	{
		return P_Atom{::xcb_intern_atom_reply(connection, ::xcb_intern_atom(connection, only_if_exists, static_cast<uint16_t>(name.size()), name.data()), nullptr)};
	}

	constexpr Key make_button_key(xcb_keycode_t keycode) noexcept
	{
		switch (keycode)
		{
		case XCB_BUTTON_INDEX_1: return Key::Mouse1;
		case XCB_BUTTON_INDEX_2: return Key::Mouse2;
		case XCB_BUTTON_INDEX_3: return Key::Mouse3;
		case XCB_BUTTON_INDEX_4: return Key::Mouse4;
		case XCB_BUTTON_INDEX_5: return Key::Mouse5;
		}
		return Key::Null;
	}

	constexpr Key make_key(xcb_keycode_t keycode) noexcept
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
		//case 0x87: return Key::Menu; // The key between RWin and RControl.
		}
		return Key::Null;
	}
}

namespace Yttrium
{
	struct WindowBackend::EmptyCursor
	{
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

		~EmptyCursor() noexcept
		{
			::xcb_free_cursor(_connection, _cursor);
		}
	};

	struct WindowBackend::XkbContext
	{
		xcb_connection_t* const _connection;
		uint8_t _base_event = 0;
		int32_t _keyboard_id = -1;
		Y_UNIQUE_PTR(xkb_context, ::xkb_context_unref) _context;
		Y_UNIQUE_PTR(xkb_keymap, ::xkb_keymap_unref) _keymap;
		Y_UNIQUE_PTR(xkb_state, ::xkb_state_unref) _state;

		explicit XkbContext(xcb_connection_t* connection)
			: _connection{connection}
		{
			if (!::xkb_x11_setup_xkb_extension(_connection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
				XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, &_base_event, nullptr))
				throw std::runtime_error{"Unable to setup XKB"};

			const uint16_t events = XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_STATE_NOTIFY;
			const uint16_t map_parts = XCB_XKB_MAP_PART_KEY_TYPES | XCB_XKB_MAP_PART_KEY_SYMS | XCB_XKB_MAP_PART_MODIFIER_MAP | XCB_XKB_MAP_PART_EXPLICIT_COMPONENTS
				| XCB_XKB_MAP_PART_KEY_ACTIONS | XCB_XKB_MAP_PART_KEY_BEHAVIORS | XCB_XKB_MAP_PART_VIRTUAL_MODS | XCB_XKB_MAP_PART_VIRTUAL_MOD_MAP;
			const auto cookie = ::xcb_xkb_select_events_aux_checked(_connection, XCB_XKB_ID_USE_CORE_KBD, events, 0, events, map_parts, map_parts, nullptr);
			Y_UNIQUE_PTR(xcb_generic_error_t, std::free) error{::xcb_request_check(_connection, cookie)};
			if (error)
				throw std::runtime_error{"Unable to select XKB events"};

			_keyboard_id = ::xkb_x11_get_core_keyboard_device_id(_connection);
			if (_keyboard_id == -1)
				throw std::runtime_error{"Unable to get XKB core keyboard device ID"};

			_context.reset(::xkb_context_new(XKB_CONTEXT_NO_FLAGS));
			if (!_context)
				throw std::runtime_error{"Unable to create XKB context"};

			reset_keymap();
		}

		void reset_keymap()
		{
			decltype(_keymap) keymap{::xkb_x11_keymap_new_from_device(_context.get(), _connection, _keyboard_id, XKB_KEYMAP_COMPILE_NO_FLAGS)};
			if (!keymap)
				throw std::runtime_error{"Unable to create XKB keymap"};

			decltype(_state) state{::xkb_x11_state_new_from_device(keymap.get(), _connection, _keyboard_id)};
			if (!state)
				throw std::runtime_error{"Unable to create XKB state"};

			std::swap(_keymap, keymap);
			std::swap(_state, state);
		}
	};

	WindowBackend::WindowBackend(const std::string& name, WindowBackendCallbacks& callbacks)
		: _callbacks{callbacks}
	{
		int preferred_screen = 0;
		_connection.reset(::xcb_connect(nullptr, &preferred_screen));
		if (::xcb_connection_has_error(_connection.get()))
			return;

		_xkb = std::make_unique<XkbContext>(_connection.get());

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

	WindowBackend::~WindowBackend() noexcept
	{
		close();
	}

	void WindowBackend::close() noexcept
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
				{
					const auto e = reinterpret_cast<const xcb_key_press_event_t*>(event.get());
					if (const auto key = ::make_key(e->detail); key != Key::Null)
					{
						Flags<KeyEvent::Modifier> modifiers;
						if (e->state & XCB_MOD_MASK_SHIFT)
							modifiers |= KeyEvent::Modifier::Shift;
						if (e->state & XCB_MOD_MASK_CONTROL)
							modifiers |= KeyEvent::Modifier::Control;
						if (e->state & XCB_MOD_MASK_1)
							modifiers |= KeyEvent::Modifier::Alt;
						_callbacks.on_key_event(key, event_type == XCB_KEY_PRESS, modifiers);
					}
					if (event_type == XCB_KEY_PRESS)
					{
						const auto size = static_cast<size_t>(::xkb_state_key_get_utf8(_xkb->_state.get(), e->detail, nullptr, 0));
						if (size > 0)
						{
							std::string buffer(size, '\0'); // TODO: Explicit small buffer optimization.
							::xkb_state_key_get_utf8(_xkb->_state.get(), e->detail, buffer.data(), size + 1);
							buffer.erase(std::remove_if(buffer.begin(), buffer.end(), [](char c){ return to_unsigned(c) < 32 || c == 127; }), buffer.end());
							if (!buffer.empty())
								_callbacks.on_text_event(buffer);
						}
					}
				}
				break;

			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
				{
					const auto e = reinterpret_cast<const xcb_button_press_event_t*>(event.get());
					if (const auto key = ::make_button_key(e->detail); key != Key::Null)
					{
						Flags<KeyEvent::Modifier> modifiers;
						if (e->state & XCB_KEY_BUT_MASK_SHIFT)
							modifiers |= KeyEvent::Modifier::Shift;
						if (e->state & XCB_KEY_BUT_MASK_CONTROL)
							modifiers |= KeyEvent::Modifier::Control;
						if (e->state & XCB_KEY_BUT_MASK_MOD_1)
							modifiers |= KeyEvent::Modifier::Alt;
						_callbacks.on_key_event(key, event_type == XCB_BUTTON_PRESS, modifiers);
					}
				}
				break;

			case XCB_FOCUS_IN:
			case XCB_FOCUS_OUT:
				_callbacks.on_focus_event(event_type == XCB_FOCUS_IN);
				break;

			case XCB_CONFIGURE_NOTIFY:
				{
					const auto e = reinterpret_cast<const xcb_configure_notify_event_t*>(event.get());
					_size.emplace(e->width, e->height);
				}
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

			default:
				// There's only one X event for all XKB events (see https://bugs.freedesktop.org/show_bug.cgi?id=51295),
				// but all XKB events are declared as separate structures with no common union like XEvent.
				if (event_type == _xkb->_base_event)
				{
					union XkbEvent
					{
						struct
						{
							uint8_t response_type;
							uint8_t xkbType;
							uint16_t sequence;
							xcb_timestamp_t time;
							uint8_t deviceID;
						} any;
						xcb_xkb_new_keyboard_notify_event_t new_keyboard_notify;
						xcb_xkb_map_notify_event_t map_notify;
						xcb_xkb_state_notify_event_t state_notify;
					};

					const auto e = reinterpret_cast<const XkbEvent*>(event.get());
					if (e->any.deviceID == _xkb->_keyboard_id)
					{
						switch (e->any.xkbType)
						{
						case XCB_XKB_NEW_KEYBOARD_NOTIFY:
						case XCB_XKB_MAP_NOTIFY:
							_xkb->reset_keymap();
							break;
						case XCB_XKB_STATE_NOTIFY:
							::xkb_state_update_mask(_xkb->_state.get(),
								e->state_notify.baseMods, e->state_notify.latchedMods, e->state_notify.lockedMods,
								to_unsigned(e->state_notify.baseGroup), to_unsigned(e->state_notify.latchedGroup), e->state_notify.lockedGroup);
							break;
						}
					}
				}
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
		static_assert(Y_RENDERER_NULL || Y_RENDERER_VULKAN, "Not implemented");
	}
}
