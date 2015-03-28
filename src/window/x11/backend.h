#ifndef __WINDOW_X11_BACKEND_H
#define __WINDOW_X11_BACKEND_H

#include <yttrium/key.h>
#include <yttrium/renderer.h>
#include <yttrium/screen.h>
#include <yttrium/static_string.h>
#include <yttrium/vector.h>

#include <GL/glx.h>
#include <X11/Xlib.h>

namespace Yttrium
{

class WindowBackend;

typedef Pointer<WindowBackend> WindowBackendPtr;

class WindowBackend: public Pointable
{
	friend Renderer;

public:

	class Callbacks
	{
	public:

		virtual void on_focus_event(bool is_focused) = 0;
		virtual void on_key_event(Key key, bool is_pressed) = 0;
	};

public:

	WindowBackend(::Display* display, ::Window window, ::GLXContext glx_context,
		const Dim2& size, Callbacks* callbacks, Allocator* allocator);
	~WindowBackend() override;

public:

	void close();

	bool get_cursor(Dim2* cursor);

	bool get_frame_sync(bool* frame_sync);

	bool put(int left, int top, int width, int height, bool border);

	bool process_events();

	Renderer& renderer() { return _renderer; }

	bool set_cursor(const Dim2& cursor);

	bool set_frame_sync(bool frame_sync);

	void set_name(const StaticString& name);

	void show();

	void swap_buffers();

public:

	static WindowBackendPtr open(const ScreenPtr& screen, const Dim2& size, Callbacks* callbacks, Allocator* allocator);

private:

	bool initialize_renderer();

private:

	::Display*   _display;
	::Window     _window;
	::Atom       _wm_protocols;
	::Atom       _wm_delete_window;
	::GLXContext _glx_context;
	Dim2         _size;
	Callbacks*   _callbacks;
	Renderer     _renderer;
};

} // namespace Yttrium

#endif // __WINDOW_X11_BACKEND_H
