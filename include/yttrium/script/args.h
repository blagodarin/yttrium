/// \file
/// \brief Script command arguments.

#ifndef __Y_SCRIPT_ARGS_H
#define __Y_SCRIPT_ARGS_H

#include <yttrium/string.h>

#include <vector>

namespace Yttrium
{

class ScriptContext;
class ScriptParser;
class ScriptValue;

/// Script command arguments.

class Y_API ScriptArgs
{
	friend ScriptContext;
	friend ScriptParser;

public:

	///

	enum NameResolution
	{
		DontResolve, ///<
		Resolve,     ///<
	};

public:

	///

	ScriptArgs(ScriptContext& context) noexcept: _context(context) {}

public:

	///

	size_t size() const noexcept { return _values.size(); }

	///

	String string(size_t index, NameResolution resolution = DontResolve) const noexcept;

	///

	const ScriptValue *value(size_t index) const noexcept;

private:

	ScriptContext&            _context;
	std::vector<ScriptValue*> _values;
};

} // namespace Yttrium

#endif // __Y_SCRIPT_ARGS_H
