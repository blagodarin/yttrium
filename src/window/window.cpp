#include <yttrium/window.h>

#include <yttrium/image.h>
#include <yttrium/renderer/modifiers.h>
#include "../renderer/renderer.h"
#include "../system/window.h"
#include "backend.h"

namespace Yttrium
{
	class WindowPrivate final : private WindowBackendCallbacks
	{
	public:
		const std::string _name;
		WindowBackend _backend{ _name, *this };
		const std::unique_ptr<RendererImpl> _renderer = RendererImpl::create(_backend); // TODO: Store renderer by value.
		bool _is_active = false;
		Point _cursor;
		bool _is_cursor_locked = false;
		Size _size;
		bool _fullscreen = false;
		bool _keys[KeyCount] = {};
		bool _take_screenshot = false;
		std::function<void(int, int)> _on_cursor_moved;
		std::function<void(const KeyEvent&)> _on_key_event;
		std::function<void(Renderer&, const Vector2&)> _on_render;
		std::function<void(Image&&)> _on_screenshot;
		std::function<void(const UpdateEvent&)> _on_update;

		explicit WindowPrivate(const std::string& name)
			: _name(name)
		{
			const auto size = _backend.size();
			if (size)
				on_resize_event(*size);
		}

		void lock_cursor(bool lock)
		{
			_is_cursor_locked = lock;
			if (_is_cursor_locked && _is_active)
			{
				_cursor = Rect(_size).center();
				_backend.set_cursor(_cursor);
			}
		}

		bool process_events()
		{
			if (!_backend.process_events())
				return false;

			if (!_is_active)
				return true;

			Point cursor = Rect(_size).center();
			_backend.get_cursor(cursor);

			const auto dx = _cursor._x - cursor._x;
			const auto dy = cursor._y - _cursor._y;

			if (!_is_cursor_locked)
				_cursor = Rect(_size).bound(cursor);
			else
				_backend.set_cursor(_cursor);

			if (_on_cursor_moved)
				_on_cursor_moved(dx, dy);

			return true;
		}

		void set_active(bool active)
		{
			_is_active = active;
			lock_cursor(_is_cursor_locked);
		}

	private:
		void on_focus_event(bool is_focused) override
		{
			set_active(is_focused);
		}

		void on_key_event(Key key, bool is_pressed) override
		{
			bool& was_pressed = _keys[to_underlying(key)];

			KeyEvent event(key, is_pressed, is_pressed && was_pressed);

			if (key != Key::WheelUp && key != Key::WheelDown && key != Key::WheelLeft && key != Key::WheelRight)
				was_pressed = is_pressed;

			if (_keys[to_underlying(Key::LShift)] || _keys[to_underlying(Key::RShift)])
				event.modifiers |= KeyEvent::Modifier::Shift;

			if (_keys[to_underlying(Key::LControl)] || _keys[to_underlying(Key::RControl)])
				event.modifiers |= KeyEvent::Modifier::Control;

			if (_keys[to_underlying(Key::LAlt)] || _keys[to_underlying(Key::RAlt)])
				event.modifiers |= KeyEvent::Modifier::Alt;

			if (_on_key_event)
				_on_key_event(event);
		}

		void on_resize_event(const Size& size) override
		{
			_size = size;
			_renderer->set_window_size(_size);
			if (!_is_cursor_locked)
			{
				auto cursor = Rect(_size).center();
				_backend.get_cursor(cursor);
				_cursor = Rect(_size).bound(cursor);
			}
			else
				lock_cursor(true);
		}
	};

	Window::Window(const std::string& name)
		: _private(std::make_unique<WindowPrivate>(name))
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

	void Window::on_render(const std::function<void(Renderer&, const Vector2&)>& callback)
	{
		_private->_on_render = callback;
	}

	void Window::on_screenshot(const std::function<void(Image&&)>& callback)
	{
		_private->_on_screenshot = callback;
	}

	void Window::on_update(const std::function<void(const UpdateEvent&)>& callback)
	{
		_private->_on_update = callback;
	}

	Renderer& Window::renderer()
	{
		return *_private->_renderer;
	}

	void Window::run()
	{
		using namespace std::literals::chrono_literals;

		UpdateEvent update;
		int frames = 0;
		auto max_frame_time = 0ms;
		auto clock = std::chrono::high_resolution_clock::now();
		auto fps_time = 0ms;
		while (_private->process_events())
		{
			if (_private->_on_update)
				_private->_on_update(update);
			{
				_private->_renderer->clear();
				PushGpuProgram gpu_program{*_private->_renderer, _private->_renderer->program_2d()};
				Push2D projection{*_private->_renderer};
				if (_private->_on_render)
					_private->_on_render(*_private->_renderer, Vector2{_private->_cursor});
			}
			_private->_backend.swap_buffers();
			if (_private->_take_screenshot)
			{
				_private->_take_screenshot = false;
				if (_private->_on_screenshot)
					_private->_on_screenshot(_private->_renderer->take_screenshot());
			}
			++frames;
			update.milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock);
			clock += update.milliseconds;
			fps_time += update.milliseconds;
			max_frame_time = std::max(max_frame_time, update.milliseconds);
			if (fps_time >= 1s)
			{
				update.fps = frames * 1000 / fps_time.count();
				update.max_frame_time = max_frame_time;
				fps_time = 0ms;
				frames = 0;
				max_frame_time = 0ms;
			}
			const auto& renderer_statistics = _private->_renderer->reset_statistics();
			update.triangles = renderer_statistics._triangles;
			update.draw_calls = renderer_statistics._draw_calls;
			update.texture_switches = renderer_statistics._texture_switches;
			update.redundant_texture_switches = renderer_statistics._redundant_texture_switches;
			update.shader_switches = renderer_statistics._shader_switches;
			update.redundant_shader_switches = renderer_statistics._redundant_shader_switches;
		}
	}

	bool Window::set_cursor(const Point& cursor)
	{
		if (_private->_is_cursor_locked || !Rect(_private->_size).contains(cursor) || !_private->_backend.set_cursor(cursor))
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
