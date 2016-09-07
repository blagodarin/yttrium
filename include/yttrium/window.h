/// \file
/// \brief

#ifndef _include_yttrium_window_h_
#define _include_yttrium_window_h_

#include <yttrium/key.h>
#include <yttrium/memory/global.h>

namespace Yttrium
{
	class Point;
	class PointF;
	class RectF;
	class Renderer;
	class Screen;
	class ScriptContext;
	class Size;
	class StaticString;
	class String;

	template <typename> class UniquePtr;

	///
	struct UpdateEvent
	{
		/// Milliseconds to update the time by.
		int milliseconds = 0;

		/// Current updates per second (frames per second, FPS) rate, \c -1 if not yet evaluated.
		int fps = -1;

		/// Maximum frame time in milliseconds.
		int max_frame_time = 0;

		/// Number of triangles rendered.
		int triangles = 0;

		/// Number of draw calls issued.
		int draw_calls = 0;

		/// Number of texture switches.
		int texture_switches = 0;

		/// Number of redundant texture switches, i.e. those which set a texture that had already been set in that frame.
		/// \note Works only in debug mode.
		int redundant_texture_switches = 0;

		/// Number of shader switches.
		int shader_switches = 0;

		/// Number of redundant shader switches, i.e. those which set a GPU program that had already been set in that frame.
		/// \note Works only in debug mode.
		int redundant_shader_switches = 0;
	};

	///
	class Y_API WindowCallbacks
	{
	public:
		///
		virtual void on_cursor_movement(const Point& movement);

		///
		virtual void on_key_event(const KeyEvent& event);

		///
		virtual void on_render(Renderer&, const PointF& cursor);

		///
		virtual void on_update(const UpdateEvent& update);
	};

	///
	class Y_API Window
	{
	public:

		/// Creates a Window.
		/// \param script_context ScriptContext for console and GUI.
		/// \param callbacks
		/// \param allocator
		static UniquePtr<Window> create(ScriptContext&, WindowCallbacks&, Allocator& = *DefaultAllocator);

		Window() = default;
		virtual ~Window() = default;

		///
		virtual void close() = 0;

		///
		virtual Point cursor() const = 0;

		///
		virtual String& debug_text() = 0;

		///
		virtual bool is_console_visible() const = 0;

		///
		virtual bool is_cursor_locked() const = 0;

		///
		virtual bool is_debug_text_visible() const = 0;

		///
		virtual bool is_shift_pressed() const = 0;

		///
		virtual void lock_cursor(bool lock) = 0;

		///
		virtual Renderer& renderer() = 0;

		///
		virtual void run() = 0;

		///
		virtual Screen& screen() = 0;

		///
		virtual void set_console_visible(bool visible) = 0;

		///
		virtual bool set_cursor(const Point& cursor) = 0;

		///
		virtual void set_debug_text_visible(bool visible) = 0;

		///
		virtual void set_name(const StaticString& name) = 0;

		///
		virtual void show() = 0;

		///
		virtual Size size() const = 0;

		/// Take a screenshot.
		/// \param name
		/// \note The screenshot would be actually taken at the end of the frame
		/// and saved in the PNG format.
		virtual void take_screenshot(const StaticString& name) = 0;
	};
}

#endif
