#ifndef __TERMINAL_TERMINAL_H
#define __TERMINAL_TERMINAL_H

#include <yttrium/terminal.h>

#include <yttrium/screen.h>

#include "console.h"
#include "window.h"

namespace Yttrium
{

class TerminalImpl: public Terminal, private Window::Callbacks
{
public:

	TerminalImpl(const Dim2 &size, Terminal::Callbacks *callbacks, Allocator *allocator) noexcept;
	~TerminalImpl() noexcept override;

	bool initialize();

public: // Terminal

	void close() noexcept override;
	Renderer create_renderer(Allocator *allocator) noexcept override;
	Dim2 cursor() const noexcept override;
	void draw_console(RendererBuiltin *renderer) noexcept override;
	bool is_console_visible() const noexcept override;
	bool is_cursor_locked() const noexcept override;
	bool is_shift_pressed() const noexcept override;
	void lock_cursor(bool lock) noexcept override;
	char printable(Key key) const noexcept override;
	bool process_events() noexcept override;
	void resize(const Dim2 &size) noexcept override;
	void set_console_visible(bool visible) noexcept override;
	bool set_cursor(const Dim2 &cursor) noexcept override;
	void set_name(const StaticString &name) noexcept override;
	void show(Mode mode) noexcept override;
	Dim2 size() const noexcept override;

private: // Window::Callbacks

	void on_focus_event(bool is_focused) noexcept override;
	void on_key_event(Key key, bool is_pressed) noexcept override;

private:

	void set_active(bool active);

private:

	ScreenPtr            _screen;
	WindowPtr            _window;
	bool                 _is_active;
	Dim2                 _cursor;
	bool                 _is_cursor_locked;
	Dim2                 _size;
	Mode                 _mode;
	unsigned             _keys[KeyCount];
	Terminal::Callbacks *_callbacks;
	Console              _console;
	bool                 _is_console_visible;
};

} // namespace Yttrium

#endif // __TERMINAL_TERMINAL_H
