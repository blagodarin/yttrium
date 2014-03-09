#ifndef __TERMINAL_X11_WINDOW_H
#define __TERMINAL_X11_WINDOW_H

#include <yttrium/key.h>
#include <yttrium/renderer.h>
#include <yttrium/screen.h>
#include <yttrium/static_string.h>
#include <yttrium/vector.h>

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Yttrium
{

typedef Pointer<Window> WindowPtr;

class Window: public Pointable
{
	friend Renderer;

	Y_NONCOPYABLE(Window);

public:

	class Callbacks
	{
	public:

		virtual void on_focus_event(bool is_focused) noexcept = 0;
		virtual void on_key_event(Key key, bool is_pressed) noexcept = 0;
	};

public:

	Window(const ScreenPtr &screen, ::Window window, const Dim2& size,
		::GLXContext glx_context, Callbacks* callbacks, Allocator* allocator);
	~Window() override;

public:

	void close();

	// TODO: Consider moving the backend selection to Window constructor
	// because different backends may require different Window internals.

	Renderer create_renderer(Renderer::Backend backend, Allocator* allocator);

	bool get_cursor(Dim2* cursor);

	bool get_frame_sync(bool* frame_sync);

	bool put(int left, int top, int width, int height, bool border);

	bool process_events();

	Renderer renderer();

	ScreenPtr screen() const;

	bool set_cursor(const Dim2 &cursor);

	bool set_frame_sync(bool frame_sync);

	void set_name(const StaticString &name);

	void show();

	void swap_buffers();

public:

	static WindowPtr open(const ScreenPtr &screen, const Dim2 &size, Callbacks *callbacks, Allocator *allocator);

private:

	ScreenPtr _screen;

	::Display  *_display;
	::Window    _window;
	::Atom      _wm_protocols;
	::Atom      _wm_delete_window;
	GLXContext  _glx_context;

	Dim2               _size;
	Renderer::Private *_renderer; // See Renderer::Private constructor/destructor.

	Callbacks *_callbacks;
};

} // namespace Yttrium

#endif // __TERMINAL_X11_WINDOW_H
