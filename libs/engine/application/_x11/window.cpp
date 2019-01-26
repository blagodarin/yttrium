//
// Copyright 2019 Sergei Blagodarin
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

#include <yttrium/exceptions.h>
#include <yttrium/image.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include "../key_codes.h"
#include "../window_callbacks.h"

#include <cstring>

#include <X11/Xatom.h>

namespace
{
	::Window create_window(::Display* display, int screen, const ::XVisualInfo* visual_info)
	{
		const auto root_window = RootWindow(display, screen);

		::XSetWindowAttributes swa;
		swa.colormap = ::XCreateColormap(display, root_window, visual_info->visual, AllocNone);
		swa.border_pixel = 0;
		swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | FocusChangeMask | StructureNotifyMask;

		const auto window = ::XCreateWindow(display, root_window,
			0, 0, 1, 1, 0, visual_info->depth, InputOutput, visual_info->visual,
			CWBorderPixel | CWColormap | CWEventMask, &swa);

		if (window == None)
		{
			::XFreeColormap(display, swa.colormap);
			throw Yttrium::InitializationError{ "XCreateWindow failed" };
		}

		return window;
	}

	constexpr Yttrium::Key button_from_event(const ::XButtonEvent& event) noexcept
	{
		using Yttrium::Key;
		switch (event.button)
		{
		case Button1: return Key::Mouse1;
		case Button2: return Key::Mouse2;
		case Button3: return Key::Mouse3;
		case Button4: return Key::Mouse4;
		case Button5: return Key::Mouse5;
		}
		return Key::Null;
	}
}

namespace Yttrium
{
	WindowBackend::WindowBackend(WindowBackendCallbacks& callbacks)
		: _window{ _application.display(), ::create_window(_application.display(), _application.screen(), _glx.visual_info()) }
		, _callbacks{ callbacks }
	{
		_input_context.reset(::XCreateIC(_application.input_method(), XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, _window.get(), nullptr));
		if (!_input_context)
			throw InitializationError{ "XCreateIC failed" };

		::XSetWMProtocols(_application.display(), _window.get(), &_wm_delete_window, 1);
		::XDefineCursor(_application.display(), _window.get(), _empty_cursor.get());
		::XSetICFocus(_input_context.get());

		{
			const auto net_wm_state = ::XInternAtom(_application.display(), "_NET_WM_STATE", False);
			const auto net_wm_state_fullscreen = ::XInternAtom(_application.display(), "_NET_WM_STATE_FULLSCREEN", False);
			::XChangeProperty(_application.display(), _window.get(), net_wm_state, XA_ATOM, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(&net_wm_state_fullscreen), 1);
		}

		_glx.bind(_window.get());

		// Force vsync.
		if (_glx->EXT_swap_control)
			_glx->SwapIntervalEXT(_application.display(), _window.get(), _glx->EXT_swap_control_tear ? -1 : 1);
	}

	WindowBackend::~WindowBackend() noexcept = default;

	void WindowBackend::close() noexcept
	{
		if (!_window)
			return;
		_glx.unbind();
		_window.reset();
	}

	bool WindowBackend::get_cursor(Point& cursor)
	{
		if (!_window)
			return false;

		::Window root = None;
		::Window child = None;
		int root_x = 0;
		int root_y = 0;
		int window_x = 0;
		int window_y = 0;
		unsigned mask = 0;

		if (!::XQueryPointer(_application.display(), _window.get(), &root, &child, &root_x, &root_y, &window_x, &window_y, &mask))
			return false;

		cursor = { window_x, window_y };

		return true;
	}

	bool WindowBackend::process_events()
	{
		if (!_window)
			return false;
		while (!_size || ::XPending(_application.display()) > 0)
		{
			::XEvent event;
			::XNextEvent(_application.display(), &event);
			if (::XFilterEvent(&event, None))
				continue;
			switch (event.type)
			{
			case KeyPress:
			case KeyRelease:
				if (const auto key = map_linux_key_code(static_cast<std::uint8_t>(event.xkey.keycode)); key != Key::Null)
				{
					Flags<KeyEvent::Modifier> modifiers;
					if (event.xkey.state & ShiftMask)
						modifiers |= KeyEvent::Modifier::Shift;
					if (event.xkey.state & ControlMask)
						modifiers |= KeyEvent::Modifier::Control;
					if (event.xkey.state & Mod1Mask)
						modifiers |= KeyEvent::Modifier::Alt;
					_callbacks.on_key_event(key, event.type == KeyPress, {}, modifiers);
				}
				if (event.type == KeyPress)
				{
					std::array<char, 32> buffer{};
					::KeySym keysym = NoSymbol;
					Status status = 0;
					const auto count = ::Xutf8LookupString(_input_context.get(), &event.xkey, buffer.data(), buffer.size(), &keysym, &status);
					if (const auto c = static_cast<unsigned char>(buffer[0]); c >= 0x20 && c != 0x7f)
						_callbacks.on_text_input(std::string_view{ buffer.data(), static_cast<std::size_t>(count) });
				}
				break;

			case ButtonPress:
			case ButtonRelease:
				if (const auto key = ::button_from_event(event.xbutton); key != Key::Null)
				{
					Flags<KeyEvent::Modifier> modifiers;
					if (event.xbutton.state & ShiftMask)
						modifiers |= KeyEvent::Modifier::Shift;
					if (event.xbutton.state & ControlMask)
						modifiers |= KeyEvent::Modifier::Control;
					if (event.xbutton.state & Mod1Mask)
						modifiers |= KeyEvent::Modifier::Alt;
					_callbacks.on_key_event(key, event.type == ButtonPress, {}, modifiers);
				}
				break;

			case FocusIn:
			case FocusOut:
				_callbacks.on_focus_event(event.type == FocusIn);
				break;

			case ConfigureNotify:
				_size.emplace(event.xconfigure.width, event.xconfigure.height);
				_callbacks.on_resize_event(*_size);
				break;

			case ClientMessage:
				if (event.xclient.message_type == _wm_protocols
					&& event.xclient.data.l[0] == static_cast<long>(_wm_delete_window))
				{
					close();
					return false;
				}
				break;
			}
		}
		return true;
	}

	bool WindowBackend::set_cursor(const Point& cursor)
	{
		if (!_window)
			return false;
		::XWarpPointer(_application.display(), None, _window.get(), 0, 0, 0, 0, cursor._x, cursor._y);
		::XSync(_application.display(), False);
		return true;
	}

	void WindowBackend::set_icon(const Image& icon)
	{
		if (!_window)
			return;
		const auto bgra_icon = to_bgra(icon);
		const auto& bgra_info = bgra_icon.info();
		const auto property_size = 2 + bgra_info.width() * bgra_info.height();
		const auto property_buffer = std::make_unique<long[]>(property_size);
		property_buffer[0] = static_cast<long>(bgra_info.width());
		property_buffer[1] = static_cast<long>(bgra_info.height());
		auto* dst = &property_buffer[2];
		for (std::size_t y = 0; y < bgra_info.height(); ++y)
		{
			const auto* src = reinterpret_cast<const std::uint32_t*>(static_cast<const std::byte*>(bgra_icon.data()) + bgra_info.row_size() * y);
			for (std::size_t x = 0; x < bgra_info.width(); ++x)
				*dst++ = src[x];
		}
		const auto net_wm_icon = ::XInternAtom(_application.display(), "_NET_WM_ICON", False);
		::XChangeProperty(_application.display(), _window.get(), net_wm_icon, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(property_buffer.get()), static_cast<int>(property_size));
		::XSync(_application.display(), False);
	}

	void WindowBackend::set_title(const std::string& title)
	{
		if (!_window)
			return;
		XTextProperty title_property;
		auto title_data = title.c_str();
		if (Success != ::Xutf8TextListToTextProperty(_application.display(), const_cast<char**>(&title_data), 1, XUTF8StringStyle, &title_property))
			return;
		const auto net_wm_name = ::XInternAtom(_application.display(), "_NET_WM_NAME", False);
		::XSetTextProperty(_application.display(), _window.get(), &title_property, net_wm_name);
		::XFree(title_property.value);
		::XSync(_application.display(), False);
	}

	void WindowBackend::show()
	{
		if (!_window)
			return;
		::XMapRaised(_application.display(), _window.get());
		::XSync(_application.display(), False);
	}

	void WindowBackend::swap_buffers()
	{
		if (_window)
			_glx.swap_buffers(_window.get());
	}

	void WindowBackend::WindowHandle::reset() noexcept
	{
		if (_window == None)
			return;
		::XDestroyWindow(_display, _window);
		_window = None;
	}

	WindowBackend::EmptyCursor::EmptyCursor(::Display* display, ::Window window)
		: _display(display)
	{
		char data[1] = { 0 };
		const auto pixmap = ::XCreateBitmapFromData(_display, window, data, 1, 1);
		if (pixmap == None)
			throw InitializationError{ "Failed to create a pixmap for an empty cursor" };
		::XColor color;
		::memset(&color, 0, sizeof(color));
		_cursor = ::XCreatePixmapCursor(_display, pixmap, pixmap, &color, &color, 0, 0);
		::XFreePixmap(_display, pixmap);
		if (_cursor == None)
			throw InitializationError{ "Failed to create an empty cursor" };
	}

	WindowBackend::EmptyCursor::~EmptyCursor() noexcept
	{
		::XFreeCursor(_display, _cursor);
	}
}
