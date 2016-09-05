/// \file
/// \brief Script command arguments.

#ifndef _include_yttrium_script_args_h_
#define _include_yttrium_script_args_h_

#include <yttrium/global.h>
#include <yttrium/std/vector.h>

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
		ScriptArgs(ScriptContext&);

		///
		int get_int(size_t index, int default_value) const;

		///
		size_t size() const { return _values.size(); }

		///
		String string(size_t index, NameResolution resolution = DontResolve) const;

		///
		const ScriptValue* value(size_t index) const;

	private:

		ScriptContext& _context;
		StdVector<ScriptValue*> _values;

		ScriptArgs(ScriptContext& context, const StdVector<ScriptValue*>& values);
	};
}

#endif
