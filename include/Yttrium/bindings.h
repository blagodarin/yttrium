/// \file
/// \brief Input key bindings.

#ifndef __Y_BINDINGS_H
#define __Y_BINDINGS_H

#include <Yttrium/key.h>
#include <Yttrium/string.h>

#include <map> // map

namespace Yttrium
{

///

class Y_API Bindings
{
public:

	///

	typedef std::map<String, String> Map;

public:

	///

	Bindings(Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	void bind(Key key, const StaticString &action) noexcept;

	///

	bool bind(const StaticString &name, const StaticString &action) noexcept;

	///

	void bind_default(Key key, const StaticString &action) noexcept;

	///

	bool bind_default(const StaticString &name, const StaticString &action) noexcept;

	///

	bool call(Key key, ExecutionMode mode = ExecutionMode::Do) noexcept;

	///

	void clear() noexcept;

	///

	Map map() const noexcept;

	///

	void unbind(Key key) noexcept;

	///

	bool unbind(const StaticString &name) noexcept;

private:

	Allocator *_allocator;
	String     _actions[KeyType(Key::__Count)];
};

} // namespace Yttrium

#endif // __Y_BINDINGS_H
