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

	inline ScriptArgs(ScriptContext *context) noexcept;

public:

	///

	inline size_t size() const noexcept;

	///

	String string(size_t index, NameResolution resolution = DontResolve) const noexcept;

	///

	const ScriptValue *value(size_t index) const noexcept;

private:

	typedef std::vector<ScriptValue *> Values;

private:

	ScriptContext *_context;
	Values         _values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ScriptArgs::ScriptArgs(ScriptContext *context) noexcept
	: _context(context)
{
}

size_t ScriptArgs::size() const noexcept
{
	return _values.size();
}

} // namespace Yttrium

#endif // __Y_SCRIPT_ARGS_H
