#include "terminal.h"

#include <yttrium/allocator.h>
#include <yttrium/utils.h>

#include <cstring> // memset

namespace Yttrium
{

TerminalImpl::TerminalImpl(const Dim2 &size, Terminal::Callbacks *callbacks, Allocator *allocator)
	: Terminal(allocator)
	, _is_active(false)
	, _is_cursor_locked(false)
	, _size(size)
	, _mode(Windowed)
	, _callbacks(callbacks)
	, _console(*this, allocator)
	, _is_console_visible(false)
{
	::memset(_keys, 0, sizeof(_keys));
}

TerminalImpl::~TerminalImpl()
{
}

bool TerminalImpl::initialize()
{
	_screen = Screen::open(allocator());
	if (_screen.is_null())
		return false;

	_window = Window::open(_screen, _size, this, allocator());
	if (_window.is_null())
		return false;

	return true;
}

void TerminalImpl::close()
{
	_window->close();
}

Renderer TerminalImpl::create_renderer(Allocator *allocator)
{
	return _window->create_renderer(allocator ? allocator : this->allocator());
}

Dim2 TerminalImpl::cursor() const
{
	return _cursor;
}

void TerminalImpl::draw_console(RendererBuiltin *renderer)
{
	if (_is_console_visible)
	{
		const Dim2 &size = renderer->size();

		renderer->set_color(0, 0, 0, 0.5);
		renderer->draw_rectangle(0, 0, size.x + 1, 1);

		_console.render_input(renderer, 0, 0, size.x);
	}
}

bool TerminalImpl::is_console_visible() const
{
	return _is_console_visible;
}

bool TerminalImpl::is_cursor_locked() const
{
	return _is_cursor_locked;
}

bool TerminalImpl::is_shift_pressed() const
{
	return _keys[KeyType(Key::LShift)] || _keys[KeyType(Key::RShift)];
}

void TerminalImpl::lock_cursor(bool lock)
{
	_is_cursor_locked = lock;

	if (lock && _is_active)
	{
		_cursor = _size / 2;
		_window->set_cursor(_cursor);
	}
}

char TerminalImpl::printable(Key key) const
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
		if (is_shift_pressed())
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

bool TerminalImpl::process_events()
{
	if (!_window->process_events())
	{
		return false;
	}

	if (_is_active)
	{
		Dim2 cursor = _size / 2;

		_window->get_cursor(&cursor);

		Dim2 movement(_cursor.x - cursor.x, cursor.y - _cursor.y);

		if (!_is_cursor_locked)
		{
			_cursor.x = clamp(cursor.x, 0, _size.x - 1);
			_cursor.y = clamp(cursor.y, 0, _size.y - 1);
		}
		else
		{
			_window->set_cursor(_cursor);
		}

		if (_callbacks)
		{
			_callbacks->on_cursor_movement(this, movement);
		}
	}

	return true;
}

void TerminalImpl::resize(const Dim2 &size)
{
	_size = size;
	if (_is_active)
		show(_mode);
}

void TerminalImpl::set_console_visible(bool visible)
{
	_is_console_visible = visible;
}

bool TerminalImpl::set_cursor(const Dim2 &cursor)
{
	if (_is_cursor_locked
		|| cursor.x < 0 || cursor.x >= _size.x
		|| cursor.y < 0 || cursor.y >= _size.y
		|| !_window->set_cursor(cursor))
	{
		return false;
	}

	_cursor = cursor;
	return true;
}

void TerminalImpl::set_name(const StaticString &name)
{
	_window->set_name(name);
}

void TerminalImpl::show(Mode mode)
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
		_size.x = min(_size.x, screen_mode.width);
		_size.y = min(_size.y, screen_mode.height);
		corner = Dim2((screen_mode.width - _size.x) / 2, (screen_mode.height - _size.y) / 2);
	}

	_window->put(corner.x, corner.y, _size.x, _size.y, _mode != Fullscreen);
	_window->show();

	if (!_is_cursor_locked)
	{
		Dim2 cursor = _size / 2;

		_window->get_cursor(&cursor);
		_cursor.x = clamp(cursor.x, 0, _size.x - 1);
		_cursor.y = clamp(cursor.y, 0, _size.y - 1);
	}
	else
	{
		lock_cursor(true);
	}

	set_active(true);
}

Dim2 TerminalImpl::size() const
{
	return _size;
}

void TerminalImpl::on_focus_event(bool is_focused)
{
	set_active(is_focused);
}

void TerminalImpl::on_key_event(Key key, bool is_pressed)
{
	unsigned pressed = 0;

	switch (key)
	{
	case Key::WheelUp:
	case Key::WheelDown:
	case Key::WheelLeft:
	case Key::WheelRight:

		if (is_pressed)
			pressed = 1;
		break;

	default:

		if (is_pressed)
			pressed = _keys[static_cast<KeyType>(key)] + 1;
		_keys[static_cast<KeyType>(key)] = pressed;
		break;
	}

	if (_is_console_visible && is_pressed)
	{
		if (key == Key::Escape)
		{
			_is_console_visible = false;
			return;
		}
		else if (_console.process_key(key))
		{
			return;
		}
	}

	if (_callbacks)
		_callbacks->on_key_event(this, key, pressed);
}

void TerminalImpl::set_active(bool active)
{
	_is_active = active;
	if (active)
		lock_cursor(_is_cursor_locked);
}

TerminalPtr Terminal::open(const Dim2 &size, Callbacks *callbacks, Allocator *allocator)
{
	if (size.x > 0 && size.y > 0)
	{
		TerminalImpl *terminal = Y_NEW(allocator, TerminalImpl)(size, callbacks, allocator);
		if (terminal->initialize())
			return TerminalPtr(terminal);
		delete terminal;
	}
	return TerminalPtr();
}

} // namespace Yttrium
