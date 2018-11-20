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

#include <yttrium/window.h>
#include "window.h"

#include <yttrium/image.h>
#include <yttrium/renderer/modifiers.h>
#include "../renderer/pass.h"

namespace Yttrium
{
	WindowPrivate::WindowPrivate(Application& application, std::string_view name)
		: _application{ application, *this }
		, _name{ name }
	{
		const auto size = _backend.size();
		if (size)
			on_resize_event(*size);
	}

	void WindowPrivate::run()
	{
		using namespace std::literals::chrono_literals;

		UpdateEvent update;
		int frames = 0;
		auto max_frame_time = 0ms;
		auto clock = std::chrono::high_resolution_clock::now();
		auto fps_time = 0ms;
		RenderPassData render_pass_data;
		while (process_events())
		{
			if (_on_update)
				_on_update(update);
			{
				RenderPassImpl pass{ *_renderer._backend, _renderer_builtin, render_pass_data, _size };
				PushProgram program{ pass, _renderer_builtin._program_2d.get() };
				Push2D projection{ pass };
				if (_on_render)
					_on_render(pass, Vector2{ _cursor });
				const auto& statistics = pass.statistics();
				update.triangles = statistics._triangles;
				update.draw_calls = statistics._draw_calls;
				update.texture_switches = statistics._texture_switches;
				update.redundant_texture_switches = statistics._redundant_texture_switches;
				update.shader_switches = statistics._shader_switches;
				update.redundant_shader_switches = statistics._redundant_shader_switches;
				pass.draw_debug_text();
			}
			_backend.swap_buffers();
			if (_take_screenshot)
			{
				_take_screenshot = false;
				if (_on_screenshot)
					_on_screenshot(_renderer.take_screenshot(_size));
			}
			++frames;
			update.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock);
			clock += update.milliseconds;
			fps_time += update.milliseconds;
			max_frame_time = std::max(max_frame_time, update.milliseconds);
			if (fps_time >= 1s)
			{
				update.fps = static_cast<int>(frames * 1000 / fps_time.count());
				update.max_frame_time = max_frame_time;
				fps_time = 0ms;
				frames = 0;
				max_frame_time = 0ms;
			}
		}
	}

	void WindowPrivate::on_focus_event(bool is_focused)
	{
		set_active(is_focused);
	}

	void WindowPrivate::on_key_event(Key key, bool is_pressed, const std::optional<Flags<KeyEvent::Modifier>>& modifiers)
	{
		bool& was_pressed = _keys[to_underlying(key)];

		KeyEvent event(key, is_pressed, is_pressed && was_pressed);

		if (key != Key::WheelUp && key != Key::WheelDown && key != Key::WheelLeft && key != Key::WheelRight)
			was_pressed = is_pressed;

		if (!modifiers)
		{
			if (_keys[to_underlying(Key::LShift)] || _keys[to_underlying(Key::RShift)])
				event.modifiers |= KeyEvent::Modifier::Shift;

			if (_keys[to_underlying(Key::LControl)] || _keys[to_underlying(Key::RControl)])
				event.modifiers |= KeyEvent::Modifier::Control;

			if (_keys[to_underlying(Key::LAlt)] || _keys[to_underlying(Key::RAlt)])
				event.modifiers |= KeyEvent::Modifier::Alt;
		}
		else
			event.modifiers = *modifiers;

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

	void WindowPrivate::on_text_event(std::string_view text)
	{
		if (_on_text_event)
			_on_text_event(text);
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

	bool WindowPrivate::process_events()
	{
		if (!_backend.process_events())
			return false;

		if (!_is_active)
			return true;

		Point cursor = Rect{ _size }.center();
		_backend.get_cursor(cursor);

		const auto dx = _cursor._x - cursor._x;
		const auto dy = cursor._y - _cursor._y;

		if (!_is_cursor_locked)
			_cursor = Rect{ _size }.bound(cursor);
		else
			_backend.set_cursor(_cursor);

		if (_on_cursor_moved)
			_on_cursor_moved(dx, dy);

		return true;
	}

	void WindowPrivate::set_active(bool active)
	{
		_is_active = active;
		lock_cursor(_is_cursor_locked);
	}

	Window::Window(Application& application, std::string_view name)
		: _private{ std::make_unique<WindowPrivate>(application, name) }
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

	void Window::on_render(const std::function<void(RenderPass&, const Vector2&)>& callback)
	{
		_private->_on_render = callback;
	}

	void Window::on_screenshot(const std::function<void(Image&&)>& callback)
	{
		_private->_on_screenshot = callback;
	}

	void Window::on_text_event(const std::function<void(std::string_view)>& callback)
	{
		_private->_on_text_event = callback;
	}

	void Window::on_update(const std::function<void(const UpdateEvent&)>& callback)
	{
		_private->_on_update = callback;
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
