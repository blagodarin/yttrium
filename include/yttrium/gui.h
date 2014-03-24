/// \file
/// \brief GUI.

#ifndef __Y_GUI_H
#define __Y_GUI_H

#include <yttrium/key.h>
#include <yttrium/pointer.h>
#include <yttrium/rect.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

class Renderer;
class Window;

class Gui;

/// Gui pointer.

typedef Pointer<Gui> GuiPtr;

/// GUI manager.

class Y_API Gui: public Pointable
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

	~Gui() override {}

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

	virtual bool process_key(const KeyEvent& event) noexcept = 0;

	///

	virtual bool render() noexcept = 0; // TODO: 'const'.

	///

	virtual void set_cursor(const Vector2f &cursor) noexcept = 0;

public:

	///

	static GuiPtr create(Renderer &renderer, Callbacks *callbacks = nullptr,
		Allocator *allocator = DefaultAllocator) noexcept;

protected:

	Gui(Allocator *allocator) noexcept: Pointable(allocator) {}
};

} // namespace Yttrium

#endif // __Y_GUI_H
