#ifndef _include_yttrium_script_args_h_
#define _include_yttrium_script_args_h_

#include <yttrium/api.h>

#include <vector>

namespace Yttrium
{
	class ScriptCode;
	class ScriptContext;
	class ScriptValue;

	/// Script command arguments.
	class Y_CORE_API ScriptArgs
	{
	public:
		std::size_t size() const noexcept { return _values.size(); }

		const ScriptValue* operator[](std::size_t) const;

	private:
		ScriptContext& _context;
		const std::vector<ScriptValue*>& _values;
		ScriptArgs(ScriptContext& context, const std::vector<ScriptValue*>& values) : _context{context}, _values{values} {}
		friend ScriptCode;
	};
}

#endif
