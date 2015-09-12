/// \file
/// \brief Script command arguments.

#ifndef __Y_SCRIPT_ARGS_H
#define __Y_SCRIPT_ARGS_H

#include <yttrium/global.h>

#include <cstddef>
#include <vector>

namespace Yttrium
{
	class ScriptCode;
	class ScriptContext;
	class ScriptParser;
	class ScriptValue;
	class String;

	/// Script command arguments.
	class Y_API ScriptArgs
	{
		friend ScriptCode;
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
		ScriptArgs(ScriptContext& context): _context(context) {}

		///
		int get_int(size_t index, int default_value) const;

		///
		size_t size() const { return _values.size(); }

		///
		String string(size_t index, NameResolution resolution = DontResolve) const;

		///
		const ScriptValue* value(size_t index) const;

	private:

		ScriptContext&            _context;
		std::vector<ScriptValue*> _values;

		ScriptArgs(ScriptContext& context, const std::vector<ScriptValue*>& values)
			: _context(context)
			, _values(values)
		{
		}
	};
}

#endif
