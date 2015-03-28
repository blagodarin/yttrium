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
	Bindings(Allocator* allocator = DefaultAllocator);

	///
	void bind(Key key, const StaticString& action);

	///
	bool bind(const StaticString &name, const StaticString& action);

	///
	void bind_default(Key key, const StaticString& action);

	///
	bool bind_default(const StaticString& name, const StaticString& action);

	///
	bool call(Key key, ScriptCode::ExecutionMode mode = ScriptCode::Do);

	///
	void clear();

	///
	std::map<String, String> map() const;

	///
	void unbind(Key key);

	///
	bool unbind(const StaticString &name);

private:

	Allocator* _allocator;
	std::array<std::pair<String, ScriptCode>, KeyCount> _actions;
};

} // namespace Yttrium

#endif // __Y_BINDINGS_H
