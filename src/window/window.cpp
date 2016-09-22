#include <yttrium/window.h>

#include <yttrium/image.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/timer.h>
#include "../renderer/renderer.h"
#include "backend.h"

#if Y_PLATFORM_POSIX
	#include "x11/window.h"
#endif

namespace Yttrium
{
	void WindowCallbacks::on_cursor_movement(const Point&)
	{
	}

	void WindowCallbacks::on_key_event(const KeyEvent&)
	{
	}

	void WindowCallbacks::on_render(Renderer&, const PointF&)
	{
	}

	void WindowCallbacks::on_update(const UpdateEvent&)
	{
	}

	class WindowPrivate : private WindowBackendCallbacks
	{
	public:
		WindowCallbacks& _callbacks;
		Allocator& _allocator;
		const String _name;
		WindowBackend _backend{ _name, *this, _allocator };
		const UniquePtr<RendererImpl> _renderer = RendererImpl::create(_backend, _allocator);
		bool _is_active = false;
		Point _cursor;
		bool _is_cursor_locked = false;
		Size _size;
		bool _fullscreen = false;
		bool _keys[KeyCount];
		String _screenshot_filename{ &_allocator };
		Image _screenshot_image{ _allocator };

		WindowPrivate(const StaticString& name, WindowCallbacks& callbacks, Allocator& allocator)
			: _callbacks(callbacks)
			, _allocator(allocator)
			, _name(name, &_allocator)
		{
			for (bool& pressed : _keys)
				pressed = false;

			ImageFormat screenshot_format;
			screenshot_format.set_pixel_format(PixelFormat::Rgb, 24);
			screenshot_format.set_orientation(ImageOrientation::XRightYUp);
			_screenshot_image.set_format(screenshot_format); // TODO: Get the proper format from the renderer.
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

			const Point movement(_cursor.x() - cursor.x(), cursor.y() - _cursor.y());

			if (!_is_cursor_locked)
				_cursor = Rect(_size).bound(cursor);
			else
				_backend.set_cursor(_cursor);

			_callbacks.on_cursor_movement(movement);

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
			bool& was_pressed = _keys[static_cast<KeyType>(key)];

			KeyEvent event(key, is_pressed, is_pressed && was_pressed);

			if (key != Key::WheelUp && key != Key::WheelDown && key != Key::WheelLeft && key != Key::WheelRight)
				was_pressed = is_pressed;

			if (_keys[static_cast<KeyType>(Key::LShift)] || _keys[static_cast<KeyType>(Key::RShift)])
				event.modifiers |= KeyEvent::Shift;

			if (_keys[static_cast<KeyType>(Key::LControl)] || _keys[static_cast<KeyType>(Key::RControl)])
				event.modifiers |= KeyEvent::Control;

			if (_keys[static_cast<KeyType>(Key::LAlt)] || _keys[static_cast<KeyType>(Key::RAlt)])
				event.modifiers |= KeyEvent::Alt;

			_callbacks.on_key_event(event);
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

	Window::Window(const StaticString& name, WindowCallbacks& callbacks, Allocator& allocator)
		: _private(make_unique<WindowPrivate>(allocator, name, callbacks, allocator))
	{
	}

	void Window::close()
	{
		_private->_backend.close();
	}

	Point Window::cursor() const
	{
		return _private->_cursor;
	}

	bool Window::is_cursor_locked() const
	{
		return _private->_is_cursor_locked;
	}

	bool Window::is_shift_pressed() const
	{
		return _private->_keys[KeyType(Key::LShift)] || _private->_keys[KeyType(Key::RShift)];
	}

	void Window::lock_cursor(bool lock)
	{
		_private->lock_cursor(lock);
	}

	Renderer& Window::renderer()
	{
		return *_private->_renderer;
	}

	void Window::run()
	{
		UpdateEvent update;
		int frames = 0;
		int max_frame_time = 0;
		auto clock = Timer::clock();
		auto fps_time = clock;
		while (_private->process_events())
		{
			_private->_callbacks.on_update(update);
			{
				_private->_renderer->clear();
				PushGpuProgram gpu_program(*_private->_renderer, _private->_renderer->program_2d());
				Push2D projection(*_private->_renderer);
				_private->_callbacks.on_render(*_private->_renderer, PointF(_private->_cursor));
			}
			_private->_backend.swap_buffers();
			if (!_private->_screenshot_filename.is_empty())
			{
				_private->_renderer->take_screenshot(_private->_screenshot_image);
				_private->_screenshot_image.save(_private->_screenshot_filename, ImageType::Png);
				_private->_screenshot_filename.clear();
			}
			++frames;
			update.milliseconds = Timer::clock() - clock;
			clock += update.milliseconds;
			fps_time += update.milliseconds;
			max_frame_time = max(max_frame_time, update.milliseconds);
			if (fps_time >= 1000)
			{
				update.fps = frames * 1000 / fps_time;
				update.max_frame_time = max_frame_time;
				fps_time = 0;
				frames = 0;
				max_frame_time = 0;
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

	void Window::take_screenshot(const StaticString& name)
	{
		_private->_screenshot_filename = name;
	}

	Window::~Window() = default;
}
