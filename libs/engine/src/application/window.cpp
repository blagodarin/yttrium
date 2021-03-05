// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/application/window.h>
#include "window.h"

#include <yttrium/application/application.h>
#include <yttrium/image/image.h>
#include <yttrium/renderer/modifiers.h>

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

	void WindowPrivate::render(RenderReport& next_report, const RenderReport& last_report)
	{
		{
			RenderPassImpl pass{ *_renderer._backend, _renderer_builtin, _render_pass_data, _size, next_report };
			PushProgram program{ pass, _renderer_builtin._program_2d.get() };
			Push2D projection{ pass };
			if (_on_render)
				_on_render(pass, Vector2{ _cursor }, last_report);
			pass.draw_debug_text();
		}
		_backend.swap_buffers();
		if (_take_screenshot)
		{
			_take_screenshot = false;
			if (_on_screenshot)
				_on_screenshot(_renderer.take_screenshot(_size));
		}
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
		_renderer.set_window_size(_size);
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

	Window::Window(Application& application)
		: _private{ std::make_unique<WindowPrivate>(application) }
	{
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

	void Window::on_render(const std::function<void(RenderPass&, const Vector2&, const RenderReport&)>& callback)
	{
		_private->_on_render = callback;
	}

	void Window::on_screenshot(const std::function<void(Image&&)>& callback)
	{
		_private->_on_screenshot = callback;
	}

	void Window::on_text_input(const std::function<void(std::string_view)>& callback)
	{
		_private->_on_text_input = callback;
	}

	RenderManager& Window::render_manager()
	{
		return _private->_renderer;
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

	void Window::take_screenshot()
	{
		_private->_take_screenshot = true;
	}
}
