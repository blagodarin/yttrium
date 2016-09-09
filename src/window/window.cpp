#include "window.h"

#include <yttrium/log.h>
#include <yttrium/math/matrix.h>
#include <yttrium/renderer_modifiers.h>
#include <yttrium/timer.h>
#include "../gui/gui.h"
#include "../renderer/renderer.h"

#if Y_PLATFORM_POSIX
	#include "x11/screen.h"
	#include "x11/window.h"
#endif

#include <stdexcept>

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

	UniquePtr<Window> Window::create(const StaticString& name, WindowCallbacks& callbacks, Allocator& allocator)
	{
		try
		{
			return make_unique<WindowImpl>(allocator, name, callbacks, allocator);
		}
		catch (const std::runtime_error& e)
		{
			Log() << "Unable to create a Window: "_s << e.what();
			return {};
		}
	}

	WindowImpl::WindowImpl(const StaticString& name, WindowCallbacks& callbacks, Allocator& allocator)
		: _callbacks(callbacks)
		, _allocator(allocator)
		, _screen(ScreenImpl::open(_allocator))
		, _backend(WindowBackend::create(_allocator, _screen->display(), _screen->screen(), name, *this))
		, _renderer(RendererImpl::create(*_backend, _allocator))
		, _screenshot_filename(&_allocator)
		, _screenshot_image(_allocator)
	{
		for (bool& pressed : _keys)
			pressed = false;

		ImageFormat screenshot_format;
		screenshot_format.set_pixel_format(PixelFormat::Rgb, 24);
		screenshot_format.set_orientation(ImageOrientation::XRightYUp);
		_screenshot_image.set_format(screenshot_format); // TODO: Get the proper format from the renderer.
	}

	WindowImpl::~WindowImpl() = default;

	void WindowImpl::close()
	{
		_backend->close();
	}

	bool WindowImpl::is_shift_pressed() const
	{
		return _keys[KeyType(Key::LShift)] || _keys[KeyType(Key::RShift)];
	}

	void WindowImpl::lock_cursor(bool lock)
	{
		_is_cursor_locked = lock;
		if (_is_cursor_locked && _is_active)
		{
			_cursor = Rect(_size).center();
			_backend->set_cursor(_cursor);
		}
	}

	Renderer& WindowImpl::renderer()
	{
		return *_renderer;
	}

	void WindowImpl::run()
	{
		UpdateEvent update;
		int frames = 0;
		int max_frame_time = 0;
		auto clock = Timer::clock();
		auto fps_time = clock;
		while (process_events())
		{
			_callbacks.on_update(update);
			{
				_renderer->clear();
				PushGpuProgram gpu_program(*_renderer, _renderer->program_2d());
				Push2D projection(*_renderer);
				_callbacks.on_render(*_renderer, PointF(_cursor));
			}
			_backend->swap_buffers();
			if (!_screenshot_filename.is_empty())
			{
				_renderer->take_screenshot(_screenshot_image);
				_screenshot_image.save(_screenshot_filename, ImageType::Png);
				_screenshot_filename.clear();
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
			const auto& renderer_statistics = _renderer->reset_statistics();
			update.triangles = renderer_statistics._triangles;
			update.draw_calls = renderer_statistics._draw_calls;
			update.texture_switches = renderer_statistics._texture_switches;
			update.redundant_texture_switches = renderer_statistics._redundant_texture_switches;
			update.shader_switches = renderer_statistics._shader_switches;
			update.redundant_shader_switches = renderer_statistics._redundant_shader_switches;
		}
	}

	Screen& WindowImpl::screen()
	{
		return *_screen;
	}

	bool WindowImpl::set_cursor(const Point& cursor)
	{
		if (_is_cursor_locked || !Rect(_size).contains(cursor) || !_backend->set_cursor(cursor))
			return false;
		_cursor = cursor;
		return true;
	}

	void WindowImpl::show()
	{
		_backend->show();
		set_active(true);
	}

	void WindowImpl::take_screenshot(const StaticString& name)
	{
		_screenshot_filename = name;
	}

	void WindowImpl::on_focus_event(bool is_focused)
	{
		set_active(is_focused);
	}

	void WindowImpl::on_key_event(Key key, bool is_pressed)
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

	void WindowImpl::on_resize_event(const Size& size)
	{
		_size = size;
		_renderer->set_window_size(_size);
		if (!_is_cursor_locked)
		{
			auto cursor = Rect(_size).center();
			_backend->get_cursor(cursor);
			_cursor = Rect(_size).bound(cursor);
		}
		else
		{
			lock_cursor(true);
		}
	}

	bool WindowImpl::process_events()
	{
		if (!_backend->process_events())
			return false;

		if (!_is_active)
			return true;

		Point cursor = Rect(_size).center();
		_backend->get_cursor(cursor);

		const Point movement(_cursor.x() - cursor.x(), cursor.y() - _cursor.y());

		if (!_is_cursor_locked)
			_cursor = Rect(_size).bound(cursor);
		else
			_backend->set_cursor(_cursor);

		_callbacks.on_cursor_movement(movement);

		return true;
	}

	void WindowImpl::set_active(bool active)
	{
		_is_active = active;
		lock_cursor(_is_cursor_locked);
	}
}
