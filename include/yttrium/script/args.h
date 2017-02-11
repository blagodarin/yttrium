/// \file
/// \brief Script command arguments.

#ifndef _include_yttrium_script_args_h_
#define _include_yttrium_script_args_h_

#include <yttrium/api.h>

#include <vector>

namespace Yttrium
{
	class ScriptCode;
	class ScriptContext;
	class ScriptValue;
	class String;

	/// Script command arguments.
	class Y_API ScriptArgs
	{
	public:
		std::size_t size() const { return _values.size(); }

		const ScriptValue* operator[](std::size_t) const;

	private:
		ScriptContext& _context;
		const std::vector<ScriptValue*>& _values;
		ScriptArgs(ScriptContext& context, const std::vector<ScriptValue*>& values) : _context(context), _values(values) {}
		friend ScriptCode;
	};
}

#endif
