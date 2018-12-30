//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "window.h"

#include "../key_codes.h"
#include "../window_callbacks.h"

#include <algorithm>
#include <string>

#include <xcb/xcb_image.h>
#define explicit explicit_ // https://bugs.freedesktop.org/show_bug.cgi?id=74080
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>

namespace
{
	using namespace Yttrium;

	constexpr Key key_from_button(xcb_button_t button) noexcept
	{
		switch (button)
		{
		case XCB_BUTTON_INDEX_1: return Key::Mouse1;
		case XCB_BUTTON_INDEX_2: return Key::Mouse2;
		case XCB_BUTTON_INDEX_3: return Key::Mouse3;
		case XCB_BUTTON_INDEX_4: return Key::Mouse4;
		case XCB_BUTTON_INDEX_5: return Key::Mouse5;
		default: return Key::Null;
		}
	}

	auto make_atom(xcb_connection_t* connection, bool only_if_exists, std::string_view name)
	{
		return P_Atom{ ::xcb_intern_atom_reply(connection, ::xcb_intern_atom(connection, only_if_exists, static_cast<uint16_t>(name.size()), name.data()), nullptr) };
	}
}

namespace Yttrium
{
	class WindowBackend::EmptyCursor
	{
	public:
		EmptyCursor(xcb_connection_t* connection, xcb_window_t window)
			: _connection{ connection }
			, _cursor{ ::xcb_generate_id(_connection) }
		{
			uint8_t data = 0;
			const auto pixmap = ::xcb_create_pixmap_from_bitmap_data(_connection, window, &data, 1, 1, 1, 0, 0, nullptr);
			::xcb_create_cursor(_connection, _cursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
			::xcb_free_pixmap(_connection, pixmap);
			::xcb_change_window_attributes(_connection, window, XCB_CW_CURSOR, &_cursor);
		}

		~EmptyCursor() noexcept
		{
			::xcb_free_cursor(_connection, _cursor);
		}

	private:
		xcb_connection_t* const _connection;
		const xcb_cursor_t _cursor;
	};

	class WindowBackend::Keyboard
	{
	public:
		explicit Keyboard(xcb_connection_t* connection)
			: _connection{ connection }
		{
			if (!::xkb_x11_setup_xkb_extension(_connection, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
					XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, nullptr, nullptr, &_base_event, nullptr))
				throw std::runtime_error{ "Unable to setup XKB" };

			constexpr uint16_t selected_events = XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_STATE_NOTIFY;

			// Mask of all valid map parts.
			constexpr uint16_t selected_map_parts = XCB_XKB_MAP_PART_KEY_TYPES | XCB_XKB_MAP_PART_KEY_SYMS
				| XCB_XKB_MAP_PART_MODIFIER_MAP | XCB_XKB_MAP_PART_EXPLICIT_COMPONENTS | XCB_XKB_MAP_PART_KEY_ACTIONS
				| XCB_XKB_MAP_PART_KEY_BEHAVIORS | XCB_XKB_MAP_PART_VIRTUAL_MODS | XCB_XKB_MAP_PART_VIRTUAL_MOD_MAP;

			const auto cookie = ::xcb_xkb_select_events_aux_checked(_connection, XCB_XKB_ID_USE_CORE_KBD,
				selected_events, 0, selected_events, selected_map_parts, selected_map_parts, nullptr);
			UniquePtr<xcb_generic_error_t, std::free> error{ ::xcb_request_check(_connection, cookie) };
			if (error)
				throw std::runtime_error{ "Unable to select XKB events" };

			_keyboard_id = ::xkb_x11_get_core_keyboard_device_id(_connection);
			if (_keyboard_id == -1)
				throw std::runtime_error{ "Unable to get XKB core keyboard device ID" };

			_context.reset(::xkb_context_new(XKB_CONTEXT_NO_FLAGS));
			if (!_context)
				throw std::runtime_error{ "Unable to create XKB context" };

			reset_keymap();
		}

		std::string keycode_to_text(xcb_keycode_t keycode) const
		{
			const auto size = static_cast<size_t>(::xkb_state_key_get_utf8(_state.get(), keycode, nullptr, 0));
			if (!size)
				return {};
			std::string buffer(size, '\0'); // TODO: Explicit small buffer optimization.
			::xkb_state_key_get_utf8(_state.get(), keycode, buffer.data(), size + 1);
			buffer.erase(std::remove_if(buffer.begin(), buffer.end(), [](char c) { return to_unsigned(c) < 32 || c == 127; }), buffer.end());
			return buffer;
		}

		bool process_event(int event_type, const xcb_generic_event_t* event)
		{
			// There's only one X event for all XKB events (see https://bugs.freedesktop.org/show_bug.cgi?id=51295).
			if (event_type != _base_event)
				return false;

			// xkbcommon doesn't have anything like XkbAnyEvent or XkbEvent.
			union xcb_xkb_event_t
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

			const auto e = reinterpret_cast<const xcb_xkb_event_t*>(event);
			if (e->any.deviceID == _keyboard_id)
			{
				switch (e->any.xkbType)
				{
				case XCB_XKB_NEW_KEYBOARD_NOTIFY:
				case XCB_XKB_MAP_NOTIFY:
					reset_keymap();
					break;
				case XCB_XKB_STATE_NOTIFY:
					::xkb_state_update_mask(_state.get(),
						e->state_notify.baseMods, e->state_notify.latchedMods, e->state_notify.lockedMods,
						to_unsigned(e->state_notify.baseGroup), to_unsigned(e->state_notify.latchedGroup), e->state_notify.lockedGroup);
					break;
				}
			}
			return true;
		}

	private:
		void reset_keymap()
		{
			decltype(_keymap) keymap{ ::xkb_x11_keymap_new_from_device(_context.get(), _connection, _keyboard_id, XKB_KEYMAP_COMPILE_NO_FLAGS) };
			if (!keymap)
				throw std::runtime_error{ "Unable to create XKB keymap" };

			decltype(_state) state{ ::xkb_x11_state_new_from_device(keymap.get(), _connection, _keyboard_id) };
			if (!state)
				throw std::runtime_error{ "Unable to create XKB state" };

			std::swap(_keymap, keymap);
			std::swap(_state, state);
		}

	private:
		xcb_connection_t* const _connection;
		uint8_t _base_event = 0;
		int32_t _keyboard_id = -1;
		UniquePtr<xkb_context, ::xkb_context_unref> _context;
		UniquePtr<xkb_keymap, ::xkb_keymap_unref> _keymap;
		UniquePtr<xkb_state, ::xkb_state_unref> _state;
	};

	WindowBackend::WindowBackend(WindowBackendCallbacks& callbacks)
		: _callbacks{ callbacks }
	{
		_keyboard = std::make_unique<Keyboard>(_application.connection());

		_window = ::xcb_generate_id(_application.connection());

		const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
		const uint32_t list[]{
			_application.screen()->black_pixel,
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_FOCUS_CHANGE,
		};
		::xcb_create_window(_application.connection(), XCB_COPY_FROM_PARENT, _window, _application.screen()->root,
			0, 0, _application.screen()->width_in_pixels, _application.screen()->height_in_pixels, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, _application.screen()->root_visual, mask, list);

		_wm_protocols = ::make_atom(_application.connection(), true, "WM_PROTOCOLS");
		_wm_delete_window = ::make_atom(_application.connection(), false, "WM_DELETE_WINDOW");
		::xcb_change_property(_application.connection(), XCB_PROP_MODE_REPLACE, _window, _wm_protocols->atom, XCB_ATOM_ATOM, 32, 1, &_wm_delete_window->atom);

		_empty_cursor = std::make_unique<EmptyCursor>(_application.connection(), _window);

		{
			const auto net_wm_state = ::make_atom(_application.connection(), false, "_NET_WM_STATE");
			const auto net_wm_state_fullscreen = ::make_atom(_application.connection(), false, "_NET_WM_STATE_FULLSCREEN");
			::xcb_change_property(_application.connection(), XCB_PROP_MODE_REPLACE, _window, net_wm_state->atom, XCB_ATOM_ATOM, 32, 1, &net_wm_state_fullscreen->atom);
		}

		::xcb_flush(_application.connection());
	}

	WindowBackend::~WindowBackend() noexcept
	{
		close();
	}

	void WindowBackend::close() noexcept
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		::xcb_destroy_window(_application.connection(), _window);
		::xcb_flush(_application.connection());
		_window = XCB_WINDOW_NONE;
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		if (_window == XCB_WINDOW_NONE)
			return false;
		const UniquePtr<xcb_query_pointer_reply_t, std::free> reply{ ::xcb_query_pointer_reply(_application.connection(), ::xcb_query_pointer(_application.connection(), _window), nullptr) };
		if (!reply)
			return false;
		cursor = { reply->win_x, reply->win_y };
		return true;
	}

	bool WindowBackend::process_events()
	{
		if (_window == XCB_WINDOW_NONE)
			return false;

		for (;;)
		{
			const UniquePtr<xcb_generic_event_t, std::free> event{ _size ? ::xcb_poll_for_event(_application.connection()) : ::xcb_wait_for_event(_application.connection()) };
			if (!event)
				return !::xcb_connection_has_error(_application.connection());

			switch (const auto event_type = event->response_type & 0x7f)
			{
			case XCB_KEY_PRESS:
			case XCB_KEY_RELEASE:
			{
				const auto e = reinterpret_cast<const xcb_key_press_event_t*>(event.get());
				if (const auto key = map_linux_key_code(e->detail); key != Key::Null)
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
					const auto text = _keyboard->keycode_to_text(e->detail);
					if (!text.empty())
						_callbacks.on_text_input(text);
				}
			}
			break;

			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
			{
				const auto e = reinterpret_cast<const xcb_button_press_event_t*>(event.get());
				if (const auto key = ::key_from_button(e->detail); key != Key::Null)
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
				_keyboard->process_event(event_type, event.get());
			}
		}
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		if (_window == XCB_WINDOW_NONE)
			return false;
		::xcb_warp_pointer(_application.connection(), XCB_WINDOW_NONE, _window, 0, 0, 0, 0, static_cast<int16_t>(cursor._x), static_cast<int16_t>(cursor._y));
		::xcb_flush(_application.connection());
		return true;
	}

	void WindowBackend::set_title(const std::string& title)
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		const auto utf8_string = ::make_atom(_application.connection(), false, "UTF8_STRING");
		::xcb_change_property(_application.connection(), XCB_PROP_MODE_REPLACE, _window, XCB_ATOM_WM_NAME, utf8_string->atom, 8, static_cast<uint32_t>(title.size()), title.data());
		::xcb_flush(_application.connection());
	}

	void WindowBackend::show()
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		::xcb_map_window(_application.connection(), _window);
		::xcb_flush(_application.connection());
	}

	void WindowBackend::swap_buffers()
	{
		static_assert(Y_RENDERER_NULL || Y_RENDERER_VULKAN, "Not implemented");
	}
}
