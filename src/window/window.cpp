#include "window.h"

#include <yttrium/allocator.h>
#include <yttrium/matrix.h>
#include <yttrium/time.h>
#include <yttrium/utils.h>
#include "../gui/gui.h"
#include "../memory/allocatable.h"
#include "../renderer/debug_renderer.h"
#include "../renderer/renderer.h"

#include <algorithm> // min

namespace Yttrium
{
	void WindowCallbacks::on_cursor_movement(const Point&)
	{
	}

	void WindowCallbacks::on_key_event(const KeyEvent&)
	{
	}

	void WindowCallbacks::on_render_canvas(Renderer&, const RectF&, const StaticString&)
	{
	}

	void WindowCallbacks::on_update(const UpdateEvent&)
	{
	}

	std::unique_ptr<Window> Window::create(WindowCallbacks& callbacks, Allocator* allocator)
	{
		auto window = std::make_unique<WindowImpl>(callbacks, allocator);
		if (!window->initialize())
			return {};
		return std::move(window);
	}

	WindowImpl::WindowImpl(WindowCallbacks& callbacks, Allocator* allocator)
		: _allocator(allocator)
		, _is_active(false)
		, _is_cursor_locked(false)
		, _size(640, 480)
		, _mode(Windowed)
		, _callbacks(callbacks)
		, _console(allocator)
		, _screenshot_filename(_allocator)
		, _screenshot_image(_allocator)
		, _debug_text(_allocator)
	{
		for (bool& pressed : _keys)
			pressed = false;

		ImageFormat screenshot_format;
		screenshot_format.set_pixel_format(PixelFormat::Rgb, 24);
		screenshot_format.set_orientation(ImageOrientation::XRightYUp);
		_screenshot_image.set_format(screenshot_format); // TODO: Get the proper format from the renderer.
	}

	WindowImpl::~WindowImpl()
	{
	}

	bool WindowImpl::initialize()
	{
		_screen = ScreenImpl::open();
		if (!_screen)
			return false;

		_backend = WindowBackend::create(*_screen, _size, *this);
		if (!_backend)
			return false;

		_renderer = RendererImpl::create(*_backend, _allocator);
		if (!_renderer)
			return false;

		_gui.reset(new GuiImpl(*_renderer, _callbacks, _allocator));
		return true;
	}

	void WindowImpl::close()
	{
		_backend->close();
	}

	Point WindowImpl::cursor() const
	{
		return _cursor;
	}

	String& WindowImpl::debug_text()
	{
		return _debug_text;
	}

	Gui& WindowImpl::gui()
	{
		return *_gui;
	}

	bool WindowImpl::is_console_visible() const
	{
		return _console_visible;
	}

	bool WindowImpl::is_cursor_locked() const
	{
		return _is_cursor_locked;
	}

	bool WindowImpl::is_debug_text_visible() const
	{
		return _debug_text_visible;
	}

	bool WindowImpl::is_shift_pressed() const
	{
		return _keys[KeyType(Key::LShift)] || _keys[KeyType(Key::RShift)];
	}

	void WindowImpl::lock_cursor(bool lock)
	{
		_is_cursor_locked = lock;
		if (lock && _is_active)
		{
			_cursor = Point(_size.width / 2, _size.height / 2);
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
				Push2D projection(*_renderer);
				_gui->render(_cursor);
				draw_debug();
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
			max_frame_time = std::max(max_frame_time, update.milliseconds);
			if (fps_time >= 1000)
			{
				update.fps = frames * 1000 / fps_time;
				update.max_frame_time = max_frame_time;
				fps_time = 0;
				frames = 0;
				max_frame_time = 0;
			}
			const auto renderer_statistics = _renderer->reset_statistics();
			update.triangles = renderer_statistics._triangles;
			update.draw_calls = renderer_statistics._draw_calls;
			update.texture_switches = renderer_statistics._texture_switches;
			update.redundant_texture_switches = renderer_statistics._redundant_texture_switches;
		}
	}

	Screen& WindowImpl::screen()
	{
		return *_screen;
	}

	void WindowImpl::set_console_visible(bool visible)
	{
		_console_visible = visible;
	}

	bool WindowImpl::set_cursor(const Point& cursor)
	{
		if (_is_cursor_locked
			|| cursor.x < 0 || cursor.x >= _size.width
			|| cursor.y < 0 || cursor.y >= _size.height
			|| !_backend->set_cursor(cursor))
		{
			return false;
		}

		_cursor = cursor;
		return true;
	}

	void WindowImpl::set_debug_text_visible(bool visible)
	{
		_debug_text_visible = visible;
	}

	void WindowImpl::set_name(const StaticString& name)
	{
		_backend->set_name(name);
	}

	void WindowImpl::set_size(const Size& size)
	{
		_size = size;
		if (_is_active)
			show(_mode);
	}

	void WindowImpl::show(Mode mode)
	{
		Point top_left;

		_mode = mode;

		if (_mode == Fullscreen)
		{
			// TODO: Set display mode here.
			const ScreenMode& screen_mode = _screen->current_mode();
			_size.width = screen_mode.width;
			_size.height = screen_mode.height;
			top_left = Point(0, 0);
		}
		else
		{
			// TODO: Restore display mode here.
			const ScreenMode& screen_mode = _screen->current_mode();
			_size.width = std::min(_size.width, screen_mode.width);
			_size.height = std::min(_size.height, screen_mode.height);
			top_left = Point((screen_mode.width - _size.width) / 2, (screen_mode.height - _size.height) / 2);
		}

		_backend->put(top_left.x, top_left.y, _size.width, _size.height, _mode != Fullscreen);
		_renderer->set_window_size(_size);
		_backend->show();

		if (!_is_cursor_locked)
		{
			Point cursor(_size.width / 2, _size.height / 2);
			_backend->get_cursor(cursor);
			_cursor.x = std::min(std::max(cursor.x, 0), _size.width - 1);
			_cursor.y = std::min(std::max(cursor.y, 0), _size.height - 1);
		}
		else
		{
			lock_cursor(true);
		}

		set_active(true);
	}

	Size WindowImpl::size() const
	{
		return _size;
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

		if (_console_visible && is_pressed)
		{
			if (key == Key::Escape)
			{
				_console_visible = false;
				return;
			}
			else if (_console.process_key(event))
			{
				return;
			}
		}

		if (_gui->process_key_event(event))
			return;

		_callbacks.on_key_event(event);
	}

	void WindowImpl::draw_debug()
	{
		const bool draw_console = _console_visible;
		const bool draw_debug_text = _debug_text_visible && !_debug_text.is_empty();

		if (!draw_console && !draw_debug_text)
			return;

		DebugRenderer renderer(*_renderer);
		const auto max_width = renderer.max_width();
		int debug_text_top = 0;

		if (draw_console)
		{
			renderer.set_color(0, 0, 0, 0.5);
			renderer.draw_rectangle(0, 0, max_width + 1, 1);
			_console.draw_input(renderer, 0, 0, max_width);
			++debug_text_top;
		}

		if (draw_debug_text)
		{
			renderer.set_color(1, 1, 1);
			size_t line_begin = 0;
			auto line_end = _debug_text.find_first('\n', line_begin);
			while (line_end != StaticString::End)
			{
				renderer.draw_text(0, debug_text_top, _debug_text.mid(line_begin, line_end - line_begin));
				++debug_text_top;
				line_begin = line_end + 1;
				line_end = _debug_text.find_first('\n', line_begin);
			}
			renderer.draw_text(0, debug_text_top, _debug_text.mid(line_begin));
		}
	}

	bool WindowImpl::process_events()
	{
		if (!_backend->process_events())
			return false;

		if (!_is_active)
			return true;

		Point cursor(_size.width / 2, _size.height / 2);
		_backend->get_cursor(cursor);

		const Point movement(_cursor.x - cursor.x, cursor.y - _cursor.y);

		if (!_is_cursor_locked)
		{
			_cursor.x = std::min(std::max(cursor.x, 0), _size.width - 1);
			_cursor.y = std::min(std::max(cursor.y, 0), _size.height - 1);
		}
		else
		{
			_backend->set_cursor(_cursor);
		}

		_callbacks.on_cursor_movement(movement);

		return true;
	}

	void WindowImpl::set_active(bool active)
	{
		_is_active = active;
		if (active)
			lock_cursor(_is_cursor_locked);
	}
}
