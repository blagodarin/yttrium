#ifndef _src_window_window_h_
#define _src_window_window_h_

#include <yttrium/window.h>

#include <yttrium/image.h>
#include <yttrium/pointer.h>
#include "backend.h"
#include "console.h"

namespace Yttrium
{
	class GuiImpl;
	class RendererImpl;
	class Screen;

	class WindowImpl: public Window, private WindowBackend::Callbacks
	{
	public:

		WindowImpl(WindowCallbacks& callbacks, Allocator* allocator);
		~WindowImpl() override;

		bool initialize();

		// Window
		void close() override;
		Point cursor() const override;
		String& debug_text() override;
		Gui& gui() override;
		bool is_console_visible() const override;
		bool is_cursor_locked() const override;
		bool is_debug_text_visible() const override;
		bool is_shift_pressed() const override;
		void lock_cursor(bool lock) override;
		Renderer& renderer() override;
		void run() override;
		Screen& screen() override;
		void set_console_visible(bool visible) override;
		bool set_cursor(const Point& cursor) override;
		void set_debug_text_visible(bool visible) override;
		void set_name(const StaticString& name) override;
		void set_size(const Size& size) override;
		void show(Mode mode) override;
		Size size() const override;
		void take_screenshot(const StaticString& name) override;

	private:

		// WindowBackend::Callbacks
		void on_focus_event(bool is_focused) override;
		void on_key_event(Key key, bool is_pressed) override;

		void draw_debug();
		bool process_events();
		void set_active(bool active);

	private:

		Allocator* const       _allocator;
		Pointer<ScreenImpl>    _screen;
		Pointer<WindowBackend> _backend;
		Pointer<RendererImpl>  _renderer;
		bool                   _is_active;
		Point                  _cursor;
		bool                   _is_cursor_locked;
		Size                   _size;
		Mode                   _mode;
		bool                   _keys[KeyCount];
		WindowCallbacks&       _callbacks;
		Console                _console;
		bool                   _console_visible = false;
		Pointer<GuiImpl>       _gui;
		String                 _screenshot_filename;
		Image                  _screenshot_image;
		String                 _debug_text;
		bool                   _debug_text_visible = false;
	};
}

#endif
