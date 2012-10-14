/// \file
/// \brief GUI manager.

#ifndef __Y_GUI_MANAGER_H
#define __Y_GUI_MANAGER_H

#include <Yttrium/key.h>
#include <Yttrium/object.h>
#include <Yttrium/static_string.h>
#include <Yttrium/vector.h>

namespace Yttrium
{

class Renderer;
class Terminal;

/// GUI facilities.

namespace Gui
{

class Manager;

/// Manager pointer.

typedef ObjectPointer<Manager> ManagerPtr;

/// GUI widget manager.

class Y_API Manager
	: public Object
{
public:

	///

	virtual void clear() noexcept = 0;

	///

	virtual bool has_scene(const StaticString &name) const noexcept = 0;

	///

	virtual bool load(const StaticString &source) noexcept = 0;

	///

	virtual bool pop_scenes(size_t count) noexcept = 0;

	///

	virtual bool push_scene(const StaticString &name) noexcept = 0;

	///

	virtual bool process_key(Terminal *terminal, Key key, KeyState state) noexcept = 0;

	///

	virtual bool render() noexcept = 0; // TODO: 'const'.

	///

	virtual void set_cursor(const Vector2f &cursor) noexcept = 0;

public:

	///

	static ManagerPtr create(Renderer &renderer, Allocator *allocator = DefaultAllocator) noexcept;

protected:

	inline Manager(Allocator *allocator) noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Manager::Manager(Allocator *allocator) noexcept
	: Object(allocator)
{
}

} // namespace Gui

} // namespace Yttrium

#endif // __Y_GUI_MANAGER_H
