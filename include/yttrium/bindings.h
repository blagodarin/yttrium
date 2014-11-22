/// \file
/// \brief Input key bindings.

#ifndef __Y_BINDINGS_H
#define __Y_BINDINGS_H

#include <yttrium/key.h>
#include <yttrium/script/code.h>
#include <yttrium/string.h>

#include <map>

namespace Yttrium
{

///

class Y_API Bindings
{
public:

	///
	Bindings(Allocator* allocator = DefaultAllocator) noexcept;

	///
	void bind(Key key, const StaticString& action) noexcept;

	///
	bool bind(const StaticString &name, const StaticString& action) noexcept;

	///
	void bind_default(Key key, const StaticString& action) noexcept;

	///
	bool bind_default(const StaticString& name, const StaticString& action) noexcept;

	///
	bool call(Key key, ScriptCode::ExecutionMode mode = ScriptCode::Do) noexcept;

	///
	void clear() noexcept;

	///
	std::map<String, String> map() const noexcept;

	///
	void unbind(Key key) noexcept;

	///
	bool unbind(const StaticString &name) noexcept;

private:

	Allocator* _allocator;
	std::array<std::pair<String, ScriptCode>, KeyCount> _actions;
};

} // namespace Yttrium

#endif // __Y_BINDINGS_H
