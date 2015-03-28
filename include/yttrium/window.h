/// \file
/// \brief

#ifndef __Y_WINDOW_H
#define __Y_WINDOW_H

#include <yttrium/key.h>
#include <yttrium/pointer.h>
#include <yttrium/vector.h>

namespace Yttrium
{

class Gui;
class Renderer;
class RendererBuiltin;
class StaticString;
class Window;

/// Window pointer.
typedef Pointer<Window> WindowPtr;

///
class Y_API Window: public Pointable
{
public:

	///
	class Callbacks
	{
	public:

		///
		virtual void on_cursor_movement(Window& window, const Dim2& movement) = 0;

		///
		virtual void on_key_event(const KeyEvent& event) = 0;

		///
		virtual void on_render_canvas(Renderer& renderer, const RectF& rect, const StaticString& canvas_name) = 0;
	};

	///
	enum Mode
	{
		Windowed,   ///<
		Fullscreen, ///<
	};

public:

	~Window() = default;

	///
	virtual void close() = 0;

	///
	virtual Dim2 cursor() const = 0;

	///
	virtual void draw_console(RendererBuiltin& renderer) = 0;

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

	/// Process window events.
	/// \return \c false if the window was closed, \c true otherwise.
	virtual bool process_events() = 0;

	///
	virtual Renderer& renderer() = 0;

	///
	virtual void resize(const Dim2& size) = 0;

	/**
	* \overload
	* \param width
	* \param height
	*/
	void resize(int width, int height)
	{
		resize(Dim2(width, height));
	}

	///
	virtual void set_console_visible(bool visible) = 0;

	///
	virtual bool set_cursor(const Dim2& cursor) = 0;

	/**
	* \overload
	* \param left
	* \param top
	* \return
	*/
	bool set_cursor(int left, int top)
	{
		return set_cursor(Dim2(left, top));
	}

	///
	virtual void set_name(const StaticString& name) = 0;

	///
	virtual void show(Mode mode = Windowed) = 0;

	///
	virtual Dim2 size() const = 0;

public:

	///
	static WindowPtr create(const Dim2& size, Callbacks& callbacks, Allocator* allocator = DefaultAllocator);

protected:

	Window(Allocator* allocator): Pointable(allocator) {}
};

} // namespace Yttrium

#endif // __Y_WINDOW_H
