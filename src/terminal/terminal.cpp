#include <Yttrium/terminal.h>

#include <cstring> // memset

namespace Yttrium
{

Terminal::Terminal(Callbacks *callbacks, Allocator *allocator)
	: _allocator(allocator)
	, _is_opened(false)
	, _is_active(false)
	, _is_cursor_locked(false)
	, _size(320, 240) // NOTE: Magic default.
	, _mode(Windowed)
	, _callbacks(callbacks)
{
	memset(_keys, 0, sizeof(_keys));
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

char Terminal::printable(Key key)
{
	// TODO: Update this dumb English-bound implementation.

	static const char char_count = 0x30;

	static const char lo_map[char_count] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', ',', '.', ';','\'',
		'/','\\', '[', ']', '-', '=', '`', ' ',
	};

	static const char hi_map[char_count] =
	{
		')', '!', '@', '#', '$', '%', '^', '&',
		'*', '(', 'A', 'B', 'C', 'D', 'E', 'F',
		'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
		'W', 'X', 'Y', 'Z', '<', '>', ':', '"',
		'?', '|', '{', '}', '_', '+', '~', ' ',
	};

	if (key >= Key::_0 && key <= Key::Space)
	{
		if (_keys[KeyType(Key::LShift)] || _keys[KeyType(Key::RShift)])
		{
			return hi_map[KeyType(key) - KeyType(Key::_0)];
		}
		else
		{
			return lo_map[KeyType(key) - KeyType(Key::_0)];
		}
	}
	else
	{
		return 0;
	}
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

		Dim2 movement(_cursor.x - cursor.left, cursor.top - _cursor.y);

		if (!_is_cursor_locked)
		{
			_cursor.x = clamp(cursor.left, 0, _size.width - 1);
			_cursor.y = clamp(cursor.top, 0, _size.height - 1);
		}
		else
		{
			_window.set_cursor(_cursor);
		}

		if (_callbacks)
		{
			_callbacks->on_cursor_movement(this, movement);
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

void Terminal::on_focus_event(Window *, bool is_focused)
{
	set_active(is_focused);
}

void Terminal::on_key_event(Window *, Key key, bool is_pressed)
{
	KeyState state;

	switch (key)
	{
	case Key::WheelUp:
	case Key::WheelDown:
	case Key::WheelLeft:
	case Key::WheelRight:

		state = (is_pressed ? 1 : 0);
		break;

	default:

		state = (is_pressed ? _keys[static_cast<KeyType>(key)] + 1 : 0);
		_keys[static_cast<KeyType>(key)] = state;
		break;
	}

	if (false) // TODO: Notify GUI.
	{
		return;
	}

	if (_callbacks && _callbacks->on_key_event(this, key, state))
	{
		return;
	}

	if (state <= 1 && false) // TODO: Notify scripted bindings.
	{
		return;
	}

	// If we're here, noone wants this key. ='(
}

} // namespace Yttrium
