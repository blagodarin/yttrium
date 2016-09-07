/// \file
/// \brief Script command arguments.

#ifndef _include_yttrium_script_args_h_
#define _include_yttrium_script_args_h_

#include <yttrium/std/vector.h>

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

		///
		enum NameResolution
		{
			DontResolve, ///<
			Resolve,     ///<
		};

	public:

		///
		int get_int(size_t index, int default_value) const;

		/// Returns the number of values.
		size_t size() const { return _values.size(); }

		///
		String string(size_t index, NameResolution = DontResolve) const;

		/// Returns value at given index or \c nullptr if no such value exist.
		const ScriptValue* value(size_t index) const;

	private:
		ScriptContext& _context;
		const StdVector<ScriptValue*>& _values;
		ScriptArgs(ScriptContext& context, const StdVector<ScriptValue*>& values) : _context(context), _values(values) {}
		friend ScriptCode;
	};
}

#endif
