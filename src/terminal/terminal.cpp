#include <Yttrium/terminal.h>

namespace Yttrium
{

Terminal::Terminal(Allocator* allocator)
	: _allocator(allocator)
	, _is_opened(false)
	, _is_active(false)
	, _is_cursor_locked(false)
	, _size(320, 240) // NOTE: Magic default.
	, _mode(Windowed)
{
}

void Terminal::close()
{
	_window.terminate();
}

void Terminal::lock_cursor(bool lock)
{
	_is_cursor_locked = lock;

	if (lock && _is_active)
	{
		_cursor = _size / 2;
		_window.set_cursor(_cursor);
	}
}

bool Terminal::open()
{
	if (!_is_opened)
	{
		if (_screen.open(_allocator))
		{
			if (_window.open(_screen, this, _allocator))
			{
				_is_opened = true;
				return true;
			}
			_screen.close();
		}
	}

	return false;
}

bool Terminal::process_events()
{
	if (!_window.process_events())
	{
		return false;
	}

	if (_is_active)
	{
		Dim2 cursor = _size / 2;

		_window.get_cursor(&cursor);
		// TODO: Handle mouse movement: Dim2(_cursor.x - cursor.left, cursor.top - _cursor.y).
		if (!_is_cursor_locked)
		{
			_cursor.x = clamp(cursor.left, 0, _size.width - 1);
			_cursor.y = clamp(cursor.top, 0, _size.height - 1);
		}
		else
		{
			_window.set_cursor(_cursor);
		}
	}

	return true;
}

void Terminal::resize(const Dim2 &size)
{
	_size = size;

	if (_is_active)
	{
		show(_mode);
	}
}

bool Terminal::set_cursor(const Dim2 &cursor)
{
	if (_is_cursor_locked
		|| cursor.x < 0 || cursor.x >= _size.width
		|| cursor.y < 0 || cursor.y >= _size.height
		|| !_window.set_cursor(cursor))
	{
		return false;
	}

	_cursor = cursor;
	return true;
}

void Terminal::show(Mode mode)
{
	ScreenMode      screen_mode;
	Window::PutMode put_mode;
	Dim2            corner;

	_mode = mode;

	if (_mode == Fullscreen)
	{
		// TODO: Set display mode here.
		screen_mode = _screen.mode();
		_size.width = screen_mode.width;
		_size.height = screen_mode.height;
		put_mode = Window::NoBorder;
		corner = Dim2(0, 0);
	}
	else
	{
		// TODO: restore display mode here.
		screen_mode = _screen.mode();
		_size.width = min(_size.width, screen_mode.width);
		_size.height = min(_size.height, screen_mode.height);
		put_mode = Window::OuterBorder;
		corner = Dim2((screen_mode.width - _size.width) / 2, (screen_mode.height - _size.height) / 2);
	}

	_window.put(corner.x, corner.y, _size.width, _size.height, put_mode);
	_window.show(Window::Focus);
	// TODO: Update renderer viewport here.

	if (!_is_cursor_locked)
	{
		Dim2 cursor = _size / 2;

		_window.get_cursor(&cursor);
		_cursor.x = clamp(cursor.left, 0, _size.width - 1);
		_cursor.y = clamp(cursor.top, 0, _size.height - 1);
	}
	else
	{
		lock_cursor(true);
	}

	set_active(true);
}

void Terminal::set_active(bool active)
{
	_is_active = active;

	if (active)
	{
		lock_cursor(_is_cursor_locked);
	}
}

void Terminal::on_focus_event(Window *window, bool is_focused)
{
	set_active(is_focused);
}

void Terminal::on_key_event(Window *window, Key key, bool is_pressed)
{
	// TODO: Handle input key.
}

} // namespace Yttrium
