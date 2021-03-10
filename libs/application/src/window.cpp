// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/window.h>
#include "window.h"

#include <yttrium/application/application.h>
#include <yttrium/math/rect.h>

// TODO: Windowed mode.

namespace Yt
{
	WindowPrivate::WindowPrivate(Application& application)
		: _application{ application, *this }
	{
		const auto size = _backend.size();
		if (size)
			on_resize_event(*size);
	}

	void WindowPrivate::update()
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

		if (_on_cursor_moved)
			_on_cursor_moved(dx, dy);
	}

	void WindowPrivate::on_focus_event(bool is_focused)
	{
		set_active(is_focused);
	}

	void WindowPrivate::on_key_event(Key key, bool pressed, bool autorepeat, Flags<KeyEvent::Modifier> modifiers)
	{
		KeyEvent event{ key, pressed, autorepeat };
		event._modifiers = modifiers;
		if (_on_key_event)
			_on_key_event(event);
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
		if (_on_text_input)
			_on_text_input(text);
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
		: _private{ std::make_unique<WindowPrivate>(application) }
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

	void Window::on_cursor_moved(const std::function<void(int, int)>& callback)
	{
		_private->_on_cursor_moved = callback;
	}

	void Window::on_key_event(const std::function<void(const KeyEvent&)>& callback)
	{
		_private->_on_key_event = callback;
	}

	void Window::on_text_input(const std::function<void(std::string_view)>& callback)
	{
		_private->_on_text_input = callback;
	}

	bool Window::set_cursor(const Point& cursor)
	{
		if (_private->_is_cursor_locked || !Rect{ _private->_size }.contains(cursor) || !_private->_backend.set_cursor(cursor))
			return false;
		_private->_cursor = cursor;
		return true;
	}

	void Window::set_icon(const Image& icon)
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
