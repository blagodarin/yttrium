#include "window.h"

#include <yttrium/allocator.h>
#include <yttrium/time.h> // RateCounter
#include <yttrium/utils.h>

#include "../gui/gui.h"
#include "../memory/allocatable.h"

#include <algorithm> // min

namespace Yttrium
{

void Window::Callbacks::on_cursor_movement(const Dim2&)
{
}

void Window::Callbacks::on_key_event(const KeyEvent&)
{
}

void Window::Callbacks::on_render_canvas(Renderer&, const RectF&, const StaticString&)
{
}

void Window::Callbacks::on_update()
{
}

std::unique_ptr<Window> Window::create(const Dim2& size, Callbacks& callbacks, Allocator* allocator)
{
	if (size.x <= 0 && size.y <= 0)
		return {};
	auto window = std::make_unique<WindowImpl>(size, callbacks, allocator);
	if (!window->initialize())
		return {};
	return std::move(window);
}

WindowImpl::WindowImpl(const Dim2& size, Window::Callbacks& callbacks, Allocator* allocator)
	: _allocator(allocator)
	, _is_active(false)
	, _is_cursor_locked(false)
	, _size(size)
	, _mode(Windowed)
	, _callbacks(callbacks)
	, _console(*this, allocator)
	, _is_console_visible(false)
{
	for (bool& pressed : _keys)
		pressed = false;
}

WindowImpl::~WindowImpl()
{
}

bool WindowImpl::initialize()
{
	_screen = Screen::open(_allocator);
	if (!_screen)
		return false;

	_backend = WindowBackend::open(_screen, _size, this, _allocator);
	if (!_backend)
		return false;

	_gui.reset(new GuiImpl(renderer(), _callbacks, _allocator));
	return true;
}

void WindowImpl::close()
{
	_backend->close();
}

Dim2 WindowImpl::cursor() const
{
	return _cursor;
}

Gui& WindowImpl::gui()
{
	return *_gui.get();
}

bool WindowImpl::is_console_visible() const
{
	return _is_console_visible;
}

bool WindowImpl::is_cursor_locked() const
{
	return _is_cursor_locked;
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
		_cursor = _size / 2;
		_backend->set_cursor(_cursor);
	}
}

Renderer& WindowImpl::renderer()
{
	return _backend->renderer();
}

void WindowImpl::resize(const Dim2& size)
{
	_size = size;
	if (_is_active)
		show(_mode);
}

void WindowImpl::run()
{
	RendererBuiltin renderer_builtin = renderer().renderer_builtin();

	RateCounter fps;

	fps.start();

	while (process_events())
	{
		_callbacks.on_update();

		renderer().begin_frame();
		_gui->set_cursor(cursor());
		_gui->render();
		draw_console(renderer_builtin);
		renderer().end_frame();

		fps.tick();
	}
}

void WindowImpl::set_console_visible(bool visible)
{
	_is_console_visible = visible;
}

bool WindowImpl::set_cursor(const Dim2& cursor)
{
	if (_is_cursor_locked
		|| cursor.x < 0 || cursor.x >= _size.x
		|| cursor.y < 0 || cursor.y >= _size.y
		|| !_backend->set_cursor(cursor))
	{
		return false;
	}

	_cursor = cursor;
	return true;
}

void WindowImpl::set_name(const StaticString& name)
{
	_backend->set_name(name);
}

void WindowImpl::show(Mode mode)
{
	Dim2 corner;

	_mode = mode;

	if (_mode == Fullscreen)
	{
		// TODO: Set display mode here.
		ScreenMode screen_mode = _screen->mode();
		_size.x = screen_mode.width;
		_size.y = screen_mode.height;
		corner = Dim2(0);
	}
	else
	{
		// TODO: Restore display mode here.
		ScreenMode screen_mode = _screen->mode();
		_size.x = std::min(_size.x, screen_mode.width);
		_size.y = std::min(_size.y, screen_mode.height);
		corner = Dim2((screen_mode.width - _size.x) / 2, (screen_mode.height - _size.y) / 2);
	}

	_backend->put(corner.x, corner.y, _size.x, _size.y, _mode != Fullscreen);
	_backend->show();

	if (!_is_cursor_locked)
	{
		Dim2 cursor = _size / 2;
		_backend->get_cursor(&cursor);
		_cursor.x = std::min(std::max(cursor.x, 0), _size.x - 1);
		_cursor.y = std::min(std::max(cursor.y, 0), _size.y - 1);
	}
	else
	{
		lock_cursor(true);
	}

	set_active(true);
}

Dim2 WindowImpl::size() const
{
	return _size;
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

	if (_is_console_visible && is_pressed)
	{
		if (key == Key::Escape)
		{
			_is_console_visible = false;
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

void WindowImpl::draw_console(RendererBuiltin& renderer)
{
	if (_is_console_visible)
	{
		const Dim2 &size = renderer.size();

		renderer.set_color(0, 0, 0, 0.5);
		renderer.draw_rectangle(0, 0, size.x + 1, 1);

		_console.render_input(renderer, 0, 0, size.x);
	}
}

bool WindowImpl::process_events()
{
	if (!_backend->process_events())
		return false;

	if (!_is_active)
		return true;

	Dim2 cursor = _size / 2;

	_backend->get_cursor(&cursor);

	Dim2 movement(_cursor.x - cursor.x, cursor.y - _cursor.y);

	if (!_is_cursor_locked)
	{
		_cursor.x = std::min(std::max(cursor.x, 0), _size.x - 1);
		_cursor.y = std::min(std::max(cursor.y, 0), _size.y - 1);
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

} // namespace Yttrium
