/// \file
/// \brief Terminal combining user input and visual output.

#ifndef __Y_TERMINAL_H
#define __Y_TERMINAL_H

#include <yttrium/terminal/console.h>
#include <yttrium/window.h>

namespace Yttrium
{

class RendererBuiltin;

/// Incapsulation of Screen and Window classes and input management.

class Y_API Terminal
	: private Window::Callbacks
{
public:

	///

	class Callbacks
	{
	public:

		///

		virtual void on_cursor_movement(Terminal *terminal, const Dim2 &movement) noexcept = 0;

		///
		/// \return \c true if the key was handled, \c false otherwise.

		virtual bool on_key_event(Terminal *terminal, Key key, unsigned pressed) noexcept = 0;
	};

	///

	enum Mode
	{
		Windowed,   ///<
		Fullscreen, ///<
	};

public:

	///

	Terminal(Callbacks *callbacks = nullptr, Allocator *allocator = DefaultAllocator) noexcept;

	///

	~Terminal() noexcept;

public:

	///

	void close() noexcept;

	///

	Renderer create_renderer(Renderer::Backend backend, Allocator *allocator = nullptr) noexcept
	{
		return _window.create_renderer(backend, allocator ? allocator : _allocator);
	}

	///

	Dim2 cursor() const noexcept
	{
		return _cursor;
	}

	///

	void draw_console(RendererBuiltin *renderer) noexcept;

	///

	bool is_console_visible() const noexcept
	{
		return _is_console_visible;
	}

	///

	bool is_cursor_locked() const noexcept
	{
		return _is_cursor_locked;
	}

	///

	bool is_shift_pressed() const noexcept
	{
		return _keys[KeyType(Key::LShift)] || _keys[KeyType(Key::RShift)];
	}

	///

	void lock_cursor(bool lock) noexcept;

	///

	bool open() noexcept;

	///

	char printable(Key key) const noexcept;

	/// Let the terminal process window events.
	/// \return \c false if the window was closed, \c true otherwise.

	bool process_events() noexcept;

	///

	void resize(const Dim2 &size) noexcept;

	/**
	* \overload
	* \param width
	* \param height
	*/

	void resize(Dim width, Dim height) noexcept
	{
		resize(Dim2(width, height));
	}

	///

	void set_console_visible(bool visible) noexcept
	{
		_is_console_visible = visible;
	}

	///

	bool set_cursor(const Dim2 &cursor) noexcept;

	/**
	* \overload
	* \param left
	* \param top
	* \return
	*/

	bool set_cursor(Dim left, Dim top) noexcept
	{
		return set_cursor(Dim2(left, top));
	}

	///

	void set_name(const StaticString &name) noexcept
	{
		_window.set_name(name);
	}

	///

	void show(Mode mode = Windowed) noexcept;

	///

	Dim2 size() const noexcept
	{
		return _size;
	}

private:

	Y_PRIVATE void set_active(bool active);

private: // Window::Callbacks

	Y_PRIVATE void on_focus_event(Window *window, bool is_focused) noexcept override;
	Y_PRIVATE void on_key_event(Window *window, Key key, bool is_pressed) noexcept override;

private:

	Allocator *_allocator;
	bool       _is_opened;
	Screen     _screen;
	Window     _window;
	bool       _is_active;
	Dim2       _cursor;
	bool       _is_cursor_locked;
	Dim2       _size;
	Mode       _mode;
	unsigned   _keys[KeyCount];
	Callbacks *_callbacks;
	Console    _console;
	bool       _is_console_visible;
};

} // namespace Yttrium

#endif // __Y_TERMINAL_H
