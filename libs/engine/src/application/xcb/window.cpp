//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/image/image.h>
#include <yttrium/utils/numeric.h>
#include "../key_codes.h"
#include "../window_callbacks.h"

#include <algorithm>
#include <cstring>
#include <string>

#include <xcb/xcb_image.h>
#define explicit explicit_ // https://gitlab.freedesktop.org/xorg/lib/libxcb/issues/23
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>

namespace
{
	constexpr Yt::Key key_from_button(xcb_button_t button) noexcept
	{
		switch (button)
		{
		case XCB_BUTTON_INDEX_1: return Yt::Key::Mouse1;
		case XCB_BUTTON_INDEX_2: return Yt::Key::Mouse2;
		case XCB_BUTTON_INDEX_3: return Yt::Key::Mouse3;
		case XCB_BUTTON_INDEX_4: return Yt::Key::Mouse4;
		case XCB_BUTTON_INDEX_5: return Yt::Key::Mouse5;
		default: return Yt::Key::Null;
		}
	}
}

namespace Yt
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
			primal::CPtr<xcb_generic_error_t, ::free> error{ ::xcb_request_check(_connection, cookie) };
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

		std::string_view keycode_to_text(xcb_keycode_t keycode)
		{
			const auto size = static_cast<size_t>(::xkb_state_key_get_utf8(_state.get(), keycode, nullptr, 0));
			if (!size)
				return {};
			if (size > _keycode_text_buffer.size())
				_keycode_text_buffer.resize(size);
			::xkb_state_key_get_utf8(_state.get(), keycode, _keycode_text_buffer.data(), size + 1);
			const auto begin = _keycode_text_buffer.begin();
			const auto end = std::remove_if(begin, begin + static_cast<std::string::difference_type>(size), [](char c) { return to_unsigned(c) < 32 || c == 127; });
			return { _keycode_text_buffer.data(), static_cast<size_t>(end - begin) };
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
					// cppcheck-suppress unusedStructMember
					uint8_t response_type;
					uint8_t xkbType;
					// cppcheck-suppress unusedStructMember
					uint16_t sequence;
					// cppcheck-suppress unusedStructMember
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
		primal::CPtr<xkb_context, ::xkb_context_unref> _context;
		primal::CPtr<xkb_keymap, ::xkb_keymap_unref> _keymap;
		primal::CPtr<xkb_state, ::xkb_state_unref> _state;
		std::string _keycode_text_buffer;
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

		::xcb_change_property(_application.connection(), XCB_PROP_MODE_REPLACE, _window, _wm_protocols->atom, XCB_ATOM_ATOM, 32, 1, &_wm_delete_window->atom);

		_empty_cursor = std::make_unique<EmptyCursor>(_application.connection(), _window);

		{
			const auto net_wm_state = make_atom("_NET_WM_STATE");
			const auto net_wm_state_fullscreen = make_atom("_NET_WM_STATE_FULLSCREEN");
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
		const primal::CPtr<xcb_query_pointer_reply_t, ::free> reply{ ::xcb_query_pointer_reply(_application.connection(), ::xcb_query_pointer(_application.connection(), _window), nullptr) };
		if (!reply)
			return false;
		cursor = { reply->win_x, reply->win_y };
		return true;
	}

	bool WindowBackend::process_events()
	{
		const auto do_key_event = [this](Key key, bool pressed, bool autorepeat, uint16_t state) {
			if (key == Key::Null)
				return;
			Flags<KeyEvent::Modifier> modifiers;
			if (state & XCB_MOD_MASK_SHIFT)
				modifiers |= KeyEvent::Modifier::Shift;
			if (state & XCB_MOD_MASK_CONTROL)
				modifiers |= KeyEvent::Modifier::Control;
			if (state & XCB_MOD_MASK_1)
				modifiers |= KeyEvent::Modifier::Alt;
			_callbacks.on_key_event(key, pressed, autorepeat, modifiers);
		};

		if (_window == XCB_WINDOW_NONE)
			return false;
		_events.clear();
		for (bool blocking = !_size;;)
		{
			P_Event event{ blocking ? ::xcb_wait_for_event(_application.connection()) : ::xcb_poll_for_event(_application.connection()) };
			if (!event)
				break;
			event->response_type &= 0x7f;
			if (blocking && event->response_type == XCB_CONFIGURE_NOTIFY)
				blocking = false;
			_events.emplace_back(std::move(event));
		}
		if (::xcb_connection_has_error(_application.connection()))
			return false;
		bool pending_autorepeat = false;
		for (std::size_t i = 0; i < _events.size(); ++i)
		{
			const auto* const event = _events[i].get();
			switch (event->response_type)
			{
			case XCB_KEY_PRESS: {
				const auto e = reinterpret_cast<const xcb_key_press_event_t*>(event);
				do_key_event(map_linux_key_code(e->detail), true, std::exchange(pending_autorepeat, false), e->state);
				const auto text = _keyboard->keycode_to_text(e->detail);
				if (!text.empty())
					_callbacks.on_text_input(text);
			}
			break;

			case XCB_KEY_RELEASE: {
				const auto e = reinterpret_cast<const xcb_key_release_event_t*>(event);
				if (i + 1 < _events.size() && _events[i + 1]->response_type == XCB_KEY_PRESS)
					if (const auto n = reinterpret_cast<const xcb_key_press_event_t*>(_events[i + 1].get()); n->detail == e->detail && n->time == e->time && n->event == e->event && n->state == e->state)
					{
						pending_autorepeat = true;
						break;
					}
				do_key_event(map_linux_key_code(e->detail), false, false, e->state);
			}
			break;

			case XCB_BUTTON_PRESS: {
				const auto e = reinterpret_cast<const xcb_button_press_event_t*>(event);
				do_key_event(::key_from_button(e->detail), true, false, e->state);
			}
			break;

			case XCB_BUTTON_RELEASE: {
				const auto e = reinterpret_cast<const xcb_button_release_event_t*>(event);
				do_key_event(::key_from_button(e->detail), false, false, e->state);
			}
			break;

			case XCB_FOCUS_IN:
			case XCB_FOCUS_OUT:
				_callbacks.on_focus_event(event->response_type == XCB_FOCUS_IN);
				break;

			case XCB_CONFIGURE_NOTIFY: {
				const auto e = reinterpret_cast<const xcb_configure_notify_event_t*>(event);
				_size.emplace(e->width, e->height);
			}
				_callbacks.on_resize_event(*_size);
				break;

			case XCB_CLIENT_MESSAGE:
				if (const auto e = reinterpret_cast<const xcb_client_message_event_t*>(event);
					e->type == _wm_protocols->atom && e->data.data32[0] == _wm_delete_window->atom)
				{
					close();
					return false;
				}
				break;

			default:
				_keyboard->process_event(event->response_type, event);
			}
		}
		return true;
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		if (_window == XCB_WINDOW_NONE)
			return false;
		::xcb_warp_pointer(_application.connection(), XCB_WINDOW_NONE, _window, 0, 0, 0, 0, static_cast<int16_t>(cursor._x), static_cast<int16_t>(cursor._y));
		::xcb_flush(_application.connection());
		return true;
	}

	void WindowBackend::set_icon(const Image& icon)
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		const auto property_size = 2 + icon.info().width() * icon.info().height();
		const auto property_buffer = std::make_unique<uint32_t[]>(property_size);
		property_buffer[0] = static_cast<uint32_t>(icon.info().width());
		property_buffer[1] = static_cast<uint32_t>(icon.info().height());
		if (!Image::transform(icon.info(), icon.data(), { icon.info().width(), icon.info().height(), PixelFormat::Bgra32, ImageOrientation::XRightYDown }, &property_buffer[2]))
			return;
		const auto net_wm_icon = make_atom("_NET_WM_ICON");
		::xcb_change_property(_application.connection(), XCB_PROP_MODE_REPLACE, _window, net_wm_icon->atom, XCB_ATOM_CARDINAL, 32, static_cast<uint32_t>(property_size), property_buffer.get());
		::xcb_flush(_application.connection());
	}

	void WindowBackend::set_title(const std::string& title)
	{
		if (_window == XCB_WINDOW_NONE)
			return;
		const auto net_wm_name = make_atom("_NET_WM_NAME");
		const auto utf8_string = make_atom("UTF8_STRING");
		::xcb_change_property(_application.connection(), XCB_PROP_MODE_REPLACE, _window, net_wm_name->atom, utf8_string->atom, 8, static_cast<uint32_t>(title.size()), title.data());
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

	WindowBackend::P_Atom WindowBackend::make_atom(std::string_view name)
	{
		return P_Atom{ ::xcb_intern_atom_reply(_application.connection(), ::xcb_intern_atom(_application.connection(), 0, static_cast<uint16_t>(name.size()), name.data()), nullptr) };
	}
}
