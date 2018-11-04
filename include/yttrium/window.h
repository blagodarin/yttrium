#ifndef _include_yttrium_window_h_
#define _include_yttrium_window_h_

#include <yttrium/api.h>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

namespace Yttrium
{
	class Application;
	class Image;
	class KeyEvent;
	class Point;
	class RenderManager;
	class RenderPass;
	class Size;
	class Vector2;

	///
	struct UpdateEvent
	{
		/// Milliseconds to update the time by.
		std::chrono::milliseconds milliseconds{ 0 };

		/// Current updates per second (frames per second, FPS) rate.
		std::optional<int> fps;

		/// Maximum frame time in milliseconds.
		std::chrono::milliseconds max_frame_time{ 0 };

		/// Number of triangles rendered.
		size_t triangles = 0;

		/// Number of draw calls issued.
		size_t draw_calls = 0;

		/// Number of texture switches.
		size_t texture_switches = 0;

		/// Number of redundant texture switches, i.e. those which set a texture that had already been set in that frame.
		/// \note Works only in debug mode.
		size_t redundant_texture_switches = 0;

		/// Number of shader switches.
		size_t shader_switches = 0;

		/// Number of redundant shader switches, i.e. those which set a GPU program that had already been set in that frame.
		/// \note Works only in debug mode.
		size_t redundant_shader_switches = 0;
	};

	///
	class Y_API Window
	{
	public:
		///
		Window(Application&, std::string_view name);

		///
		~Window();

		///
		void close();

		///
		Point cursor() const;

		///
		void lock_cursor(bool lock);

		///
		void on_cursor_moved(const std::function<void(int dx, int dy)>&);

		///
		void on_key_event(const std::function<void(const KeyEvent&)>&);

		///
		void on_render(const std::function<void(RenderPass&, const Vector2& cursor)>&);

		/// Sets a callback to be called when a screenshot image is ready.
		void on_screenshot(const std::function<void(Image&&)>&);

		///
		void on_text_event(const std::function<void(std::string_view)>&);

		///
		void on_update(const std::function<void(const UpdateEvent&)>&);

		///
		RenderManager& render_manager();

		///
		bool set_cursor(const Point& cursor);

		///
		void show();

		///
		Size size() const;

		/// Requests a screenshot to be taken.
		void take_screenshot();

	private:
		const std::unique_ptr<class WindowPrivate> _private;
	};
}

#endif
