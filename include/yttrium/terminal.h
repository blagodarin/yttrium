/// \file
/// \brief Terminal combining user input and visual output.

#ifndef __Y_TERMINAL_H
#define __Y_TERMINAL_H

#include <yttrium/key.h>
#include <yttrium/renderer.h>

namespace Yttrium
{

class Terminal;
class RendererBuiltin;

/// Terminal pointer.

typedef Pointer<Terminal> TerminalPtr;

/// Incapsulation of Screen and Window classes and input management.

class Y_API Terminal: public Pointable
{
public:

	///

	class Callbacks
	{
	public:

		///

		virtual void on_cursor_movement(Terminal *terminal, const Dim2 &movement) noexcept = 0;

		///
		/// \return \c true if the key was handled, \c false otherwise.

		virtual bool on_key_event(Terminal *terminal, Key key, unsigned pressed) noexcept = 0;
	};

	///

	enum Mode
	{
		Windowed,   ///<
		Fullscreen, ///<
	};

public:

	virtual ~Terminal() {}

	///

	virtual void close() noexcept = 0;

	///

	virtual Renderer create_renderer(Allocator *allocator = nullptr) noexcept = 0;

	///

	virtual Dim2 cursor() const noexcept = 0;

	///

	virtual void draw_console(RendererBuiltin *renderer) noexcept = 0;

	///

	virtual bool is_console_visible() const noexcept = 0;

	///

	virtual bool is_cursor_locked() const noexcept = 0;

	///

	virtual bool is_shift_pressed() const noexcept = 0;

	///

	virtual void lock_cursor(bool lock) noexcept = 0;

	///

	virtual char printable(Key key) const noexcept = 0;

	/// Let the terminal process window events.
	/// \return \c false if the window was closed, \c true otherwise.

	virtual bool process_events() noexcept = 0;

	///

	virtual void resize(const Dim2 &size) noexcept = 0;

	/**
	* \overload
	* \param width
	* \param height
	*/

	void resize(int width, int height) noexcept
	{
		resize(Dim2(width, height));
	}

	///

	virtual void set_console_visible(bool visible) noexcept = 0;

	///

	virtual bool set_cursor(const Dim2 &cursor) noexcept = 0;

	/**
	* \overload
	* \param left
	* \param top
	* \return
	*/

	bool set_cursor(int left, int top) noexcept
	{
		return set_cursor(Dim2(left, top));
	}

	///

	virtual void set_name(const StaticString &name) noexcept = 0;

	///

	virtual void show(Mode mode = Windowed) noexcept = 0;

	///

	virtual Dim2 size() const noexcept = 0;

public:

	///

	static TerminalPtr open(const Dim2 &size, Callbacks *callbacks = nullptr,
		Allocator *allocator = DefaultAllocator) noexcept;

protected:

	Terminal(Allocator *allocator) noexcept: Pointable(allocator) {}
};

} // namespace Yttrium

#endif // __Y_TERMINAL_H
