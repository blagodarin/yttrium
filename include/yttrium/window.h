/// \file
/// \brief

#ifndef __Y_WINDOW_H
#define __Y_WINDOW_H

#include <yttrium/key.h>
#include <yttrium/vector.h>

#include <memory>

namespace Yttrium
{

class Gui;
class Renderer;
class StaticString;

///
class Y_API Window
{
public:

	///
	class Callbacks
	{
	public:

		///
		virtual void on_cursor_movement(const Dim2& movement);

		///
		virtual void on_key_event(const KeyEvent& event);

		///
		virtual void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name);

		///
		virtual void on_update();
	};

	///
	enum Mode
	{
		Windowed,   ///<
		Fullscreen, ///<
	};

	///
	static std::unique_ptr<Window> create(const Dim2& size, Callbacks& callbacks, Allocator* allocator = DefaultAllocator);

	Window() = default;
	virtual ~Window() = default;

	///
	virtual void close() = 0;

	///
	virtual Dim2 cursor() const = 0;

	///
	virtual Gui& gui() = 0;

	///
	virtual bool is_console_visible() const = 0;

	///
	virtual bool is_cursor_locked() const = 0;

	///
	virtual bool is_shift_pressed() const = 0;

	///
	virtual void lock_cursor(bool lock) = 0;

	///
	virtual Renderer& renderer() = 0;

	///
	virtual void resize(const Dim2& size) = 0;

	///
	virtual void run() = 0;

	///
	virtual void set_console_visible(bool visible) = 0;

	///
	virtual bool set_cursor(const Dim2& cursor) = 0;

	///
	virtual void set_name(const StaticString& name) = 0;

	///
	virtual void show(Mode mode = Windowed) = 0;

	///
	virtual Dim2 size() const = 0;
};

} // namespace Yttrium

#endif // __Y_WINDOW_H
