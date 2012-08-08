/// \file
/// \brief

#ifndef __Y_WINDOW_H
#define __Y_WINDOW_H

#include <Yttrium/key.h>
#include <Yttrium/renderer.h>
#include <Yttrium/screen.h>
#include <Yttrium/static_string.h>
#include <Yttrium/vector.h>

namespace Yttrium
{

///

class Y_API Window
{
	friend class Renderer;

public:

	///

	class Callbacks
	{
	public:

		Callbacks() = default;

		///

		virtual void on_focus_event(Window *window, bool is_focused) noexcept = 0;

		///

		virtual void on_key_event(Window *window, Key key, bool is_pressed) noexcept = 0;
	};

	///

	enum PutMode
	{
		InnerBorder, ///<
		NoBorder,    ///<
		OuterBorder, ///<
	};

	///

	enum ShowMode
	{
		Hide,  ///<
		Show,  ///<
		Focus, ///<
	};

public:

	///

	Window() noexcept
		: _private(nullptr)
		, _callbacks(nullptr)
	{
	}

	///

	Window(const Screen &screen, Callbacks *callbacks, Allocator *allocator = DefaultAllocator) noexcept
		//: Window() // TODO: Uncomment.
		: _private(nullptr)
		, _callbacks(nullptr)
	{
		open(screen, callbacks, allocator);
	}

	///

	Window(const Window &window) noexcept;

	///

	~Window() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	///

	// NOTE: It may be better to move the backend selection to the Window constructor
	// because different backends may require different Window internals.

	Renderer create_renderer(Renderer::Backend backend, Allocator *allocator = nullptr) noexcept;

	///

	bool get_cursor(Dim2 *cursor) noexcept;

	///

	bool get_frame_sync(bool *frame_sync) noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	bool open(const Screen &screen, Callbacks *callbacks, Allocator *allocator = DefaultAllocator) noexcept;

	///

	bool put(Dim left, Dim top, Dim width, Dim height, PutMode mode) noexcept;

	///

	bool process_events() noexcept;

	///

	Renderer renderer() noexcept;

	///

	Screen screen() const noexcept
	{
		return _screen;
	}

	///

	bool set_cursor(const Dim2 &cursor) noexcept;

	///

	bool set_frame_sync(bool frame_sync) noexcept;

	///

	void set_name(const StaticString &name) noexcept;

	///

	void show(ShowMode mode) noexcept;

	///

	void swap_buffers() noexcept;

	///

	void terminate() noexcept;

public:

	///

	Window &operator =(const Window &window) noexcept;

public:

	///

	static bool is_supported(Renderer::Backend backend) noexcept;

private:

	class Private;

	Private   *_private;
	Callbacks *_callbacks;
	Screen     _screen;
};

} // namespace Yttrium

#endif // __Y_WINDOW_H