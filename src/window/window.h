#ifndef _src_window_window_h_
#define _src_window_window_h_

#include <yttrium/window.h>

#include <yttrium/image.h>
#include <yttrium/math/point.h>
#include <yttrium/math/size.h>
#include <yttrium/memory/unique_ptr.h>
#include <yttrium/string.h>
#include "backend.h"

namespace Yttrium
{
	class GuiPrivate;
	class RendererImpl;
	class ScreenImpl;
	class WindowBackend;

	class WindowImpl : public Window, private WindowBackendCallbacks
	{
	public:
		WindowImpl(WindowCallbacks&, Allocator&);
		~WindowImpl() override;

		// Window
		void close() override;
		Point cursor() const override { return _cursor; }
		String& debug_text() override { return _debug_text; }
		bool is_cursor_locked() const override { return _is_cursor_locked; }
		bool is_debug_text_visible() const override { return _debug_text_visible; }
		bool is_shift_pressed() const override;
		void lock_cursor(bool lock) override;
		Renderer& renderer() override;
		void run() override;
		Screen& screen() override;
		bool set_cursor(const Point& cursor) override;
		void set_debug_text_visible(bool visible) override;
		void set_name(const StaticString& name) override;
		void show() override;
		Size size() const override { return _size; }
		void take_screenshot(const StaticString& name) override;

	private:
		// WindowBackendCallbacks
		void on_focus_event(bool is_focused) override;
		void on_key_event(Key key, bool is_pressed) override;
		void on_resize_event(const Size& size) override;

		void draw_debug();
		bool process_events();
		void set_active(bool active);

	private:
		WindowCallbacks&       _callbacks;
		Allocator&             _allocator;
		UniquePtr<ScreenImpl>  _screen;
		UniquePtr<WindowBackend> _backend;
		UniquePtr<RendererImpl> _renderer;
		bool                   _is_active = false;
		Point                  _cursor;
		bool                   _is_cursor_locked = false;
		Size                   _size;
		bool                   _fullscreen = false;
		bool                   _keys[KeyCount];
		String                 _screenshot_filename;
		Image                  _screenshot_image;
		String                 _debug_text;
		bool                   _debug_text_visible = false;
	};
}

#endif
