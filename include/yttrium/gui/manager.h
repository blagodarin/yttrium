/// \file
/// \brief GUI manager.

#ifndef __Y_GUI_MANAGER_H
#define __Y_GUI_MANAGER_H

#include <yttrium/key.h>
#include <yttrium/pointer.h>
#include <yttrium/rect.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

class Renderer;
class Window;

/// GUI facilities.

namespace Gui
{

class Manager;

/// Manager pointer.

typedef Pointer<Manager> ManagerPtr;

/// GUI widget manager.

class Y_API Manager: public Pointable
{
public:

	///

	class Callbacks
	{
	public:

		///

		virtual void on_render_canvas(Renderer *renderer, const StaticString &name, const RectF &rect) noexcept = 0;
	};

public:

	///

	virtual ~Manager() noexcept
	{
	}

public:

	///

	virtual void clear() noexcept = 0;

	///

	virtual void dump(const StaticString &filename) const noexcept = 0;

	///

	virtual bool has_scene(const StaticString &name) const noexcept = 0;

	///

	virtual bool load(const StaticString &filename) noexcept = 0;

	///

	virtual bool pop_scenes(size_t count) noexcept = 0;

	///

	virtual bool push_scene(const StaticString &name) noexcept = 0;

	///

	virtual bool process_key(Window *window, Key key, unsigned pressed) noexcept = 0;

	///

	virtual bool render() noexcept = 0; // TODO: 'const'.

	///

	virtual void set_cursor(const Vector2f &cursor) noexcept = 0;

public:

	///

	static ManagerPtr create(Renderer &renderer, Callbacks *callbacks = nullptr,
		Allocator *allocator = DefaultAllocator) noexcept;

protected:

	Manager(Allocator *allocator) noexcept: Pointable(allocator) {}
};

} // namespace Gui

} // namespace Yttrium

#endif // __Y_GUI_MANAGER_H
