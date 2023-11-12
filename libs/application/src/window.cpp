// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/window.h>
#include "window.h"

#include <yttrium/application/application.h>
#include <yttrium/geometry/rect.h>

#include <cassert>

// TODO: Windowed mode.

namespace Yt
{
	WindowPrivate::WindowPrivate(Application& application, Window& window)
		: _application{ application, *this }
		, _window{ window }
	{
		const auto size = _backend.size();
		if (size)
			on_resize_event(*size);
	}

	bool WindowPrivate::process_events(EventCallbacks& callbacks)
	{
		class SetCallbacks
		{
		public:
			constexpr SetCallbacks(EventCallbacks*& dst, EventCallbacks& src) noexcept
				: _dst{ dst }, _previous{ _dst } { _dst = &src; }
			constexpr ~SetCallbacks() noexcept { _dst = _previous; }

		private:
			EventCallbacks*& _dst;
			EventCallbacks* const _previous;
		};
		assert(!_callbacks);
		SetCallbacks set_callbacks{ _callbacks, callbacks };
		return _backend.process_events();
	}

	void WindowPrivate::update(EventCallbacks& callbacks)
	{
		if (!_is_active)
			return;

		Point cursor = Rect{ _size }.center();
		_backend.get_cursor(cursor);

		// cppcheck-suppress variableScope
		const auto dx = _cursor._x - cursor._x;
		// cppcheck-suppress variableScope
		const auto dy = cursor._y - _cursor._y;

		if (!_is_cursor_locked)
			_cursor = Rect{ _size }.bound(cursor);
		else
			_backend.set_cursor(_cursor);

		if (dx != 0 || dy != 0)
			callbacks.onWindowMouseMove(_window, dx, dy);
	}

	void WindowPrivate::on_focus_event(bool is_focused)
	{
		set_active(is_focused);
	}

	void WindowPrivate::on_key_event(Key key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier> modifiers)
	{
		KeyEvent event{ key, pressed, autorepeat };
		event._modifiers = modifiers;
		_callbacks->onWindowKeyEvent(_window, event);
	}

	void WindowPrivate::on_resize_event(const Size& size)
	{
		_size = size;
		if (!_is_cursor_locked)
		{
			auto cursor = Rect(_size).center();
			_backend.get_cursor(cursor);
			_cursor = Rect(_size).bound(cursor);
		}
		else
			lock_cursor(true);
	}

	void WindowPrivate::on_text_input(std::string_view text)
	{
		_callbacks->onWindowTextInput(_window, text);
	}

	void WindowPrivate::lock_cursor(bool lock)
	{
		_is_cursor_locked = lock;
		if (_is_cursor_locked && _is_active)
		{
			_cursor = Rect(_size).center();
			_backend.set_cursor(_cursor);
		}
	}

	void WindowPrivate::set_active(bool active)
	{
		_is_active = active;
		lock_cursor(_is_cursor_locked);
	}

	Window::Window(Application& application, const std::string& title)
		: _private{ std::make_unique<WindowPrivate>(application, *this) }
	{
		set_title(title);
	}

	Window::~Window() = default;

	void Window::close()
	{
		_private->_backend.close();
	}

	Point Window::cursor() const
	{
		return _private->_cursor;
	}

	WindowID Window::id() const noexcept
	{
		return _private->_backend.id();
	}

	void Window::lock_cursor(bool lock)
	{
		_private->lock_cursor(lock);
	}

	bool Window::set_cursor(const Point& cursor)
	{
		if (_private->_is_cursor_locked || !Rect{ _private->_size }.contains(cursor) || !_private->_backend.set_cursor(cursor))
			return false;
		_private->_cursor = cursor;
		return true;
	}

	void Window::set_icon(const seir::Image& icon)
	{
		_private->_backend.set_icon(icon);
	}

	void Window::set_title(const std::string& title)
	{
		_private->_backend.set_title(title);
	}

	void Window::show()
	{
		_private->_backend.show();
		_private->set_active(true);
	}

	Size Window::size() const
	{
		return _private->_size;
	}

	void Window::swap_buffers()
	{
		_private->_backend.swap_buffers();
	}
}
