#ifndef __WINDOW_WINDOW_H
#define __WINDOW_WINDOW_H

#include <yttrium/window.h>

#include <yttrium/screen.h>

#include "backend.h"
#include "console.h"

namespace Yttrium
{

class WindowImpl: public Window, private WindowBackend::Callbacks
{
public:

	WindowImpl(const Dim2& size, Window::Callbacks& callbacks, Allocator* allocator);
	~WindowImpl() override;

	bool initialize();

public: // Window

	void close() override;
	Renderer create_renderer(Allocator* allocator) override;
	Dim2 cursor() const override;
	void draw_console(RendererBuiltin& renderer) override;
	bool is_console_visible() const override;
	bool is_cursor_locked() const override;
	bool is_shift_pressed() const override;
	void lock_cursor(bool lock) override;
	bool process_events() override;
	void resize(const Dim2& size) override;
	void set_console_visible(bool visible) override;
	bool set_cursor(const Dim2& cursor) override;
	void set_name(const StaticString& name) override;
	void show(Mode mode) override;
	Dim2 size() const override;

private: // WindowBackend::Callbacks

	void on_focus_event(bool is_focused) override;
	void on_key_event(Key key, bool is_pressed) override;

private:

	void set_active(bool active);

private:

	ScreenPtr          _screen;
	WindowBackendPtr   _backend;
	bool               _is_active;
	Dim2               _cursor;
	bool               _is_cursor_locked;
	Dim2               _size;
	Mode               _mode;
	bool               _keys[KeyCount];
	Window::Callbacks& _callbacks;
	Console            _console;
	bool               _is_console_visible;
};

} // namespace Yttrium

#endif // __WINDOW_WINDOW_H
