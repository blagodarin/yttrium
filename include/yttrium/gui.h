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

class Gui;
class Renderer;

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
		virtual void on_render_canvas(Renderer& renderer, const StaticString& name, const RectF& rect) = 0;
	};

public:

	~Gui() = default;

	///
	virtual void clear() = 0;

	///
	virtual void dump(const StaticString& filename) const = 0;

	///
	virtual bool has_scene(const StaticString& name) const = 0;

	///
	virtual bool load(const StaticString& filename) = 0;

	///
	virtual bool pop_scenes(size_t count) = 0;

	///
	virtual bool push_scene(const StaticString& name) = 0;

	///
	virtual bool process_key(const KeyEvent& event) = 0;

	///
	virtual bool render() = 0; // TODO: 'const'.

	///
	virtual void set_cursor(const Vector2f& cursor) = 0;

public:

	///
	static GuiPtr create(Renderer& renderer, Callbacks& callbacks, Allocator* allocator = DefaultAllocator);

protected:

	Gui(Allocator* allocator): Pointable(allocator) {}
};

} // namespace Yttrium

#endif // __Y_GUI_H
