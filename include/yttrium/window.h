/// \file
/// \brief

#ifndef _include_yttrium_window_h_
#define _include_yttrium_window_h_

#include <yttrium/memory/unique_ptr.h>

namespace Yttrium
{
	class KeyEvent;
	class Point;
	class PointF;
	class Renderer;
	class Size;
	class StaticString;

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
		virtual void on_key_event(const KeyEvent&);

		///
		virtual void on_render(Renderer&, const PointF& cursor);

		///
		virtual void on_update(const UpdateEvent&);
	};

	///
	class Y_API Window
	{
	public:
		///
		/// \throws std::runtime_error If unable to create a Window.
		Window(const StaticString& name, WindowCallbacks&, Allocator& = *DefaultAllocator);

		///
		void close();

		///
		Point cursor() const;

		///
		bool is_cursor_locked() const;

		///
		bool is_shift_pressed() const;

		///
		void lock_cursor(bool lock);

		///
		Renderer& renderer();

		///
		void run();

		///
		bool set_cursor(const Point& cursor);

		///
		void show();

		///
		Size size() const;

		/// Take a screenshot.
		/// \param name
		/// \note The screenshot would be actually taken at the end of the frame
		/// and saved in the PNG format.
		void take_screenshot(const StaticString& name);

	private:
		const UniquePtr<class WindowPrivate> _private;

	public:
		~Window();
	};
}

#endif
