/// \file
/// \brief Script value.

#ifndef _include_yttrium_script_value_h_
#define _include_yttrium_script_value_h_

#include <yttrium/string.h>

namespace Yttrium
{
	class ScriptArgs;
	class ScriptCode;
	class ScriptContext;
	class ScriptParser;

	/// Script value data.
	class Y_API ScriptValue
	{
		friend ScriptArgs;
		friend ScriptCode;
		friend ScriptContext;
		friend ScriptParser;

	public:

		/// Value type.
		/// \note Every Name is a Literal, and every Literal is a String, but not visa versa.
		enum class Type
		{
			String,  ///< String value.
			Literal, ///< Literal value (e.g. integer).
			Name,    ///< Name (e.g. identifier).
		};

	public:

		///
		int to_int() const { return _value.to_int32(); }

		///
		double to_double() const { return _value.to_double(); }

		///
		String to_string() const { return _value; }

		///
		Type type() const { return _type; }

	public:

		///
		ScriptValue& operator=(int value);

		///

		ScriptValue& operator=(double value);

		///

		ScriptValue& operator=(const StaticString& value);

	private:

		Type _type;
		String _value;

	private:

		Y_PRIVATE ScriptValue(int value, Allocator* allocator);
		Y_PRIVATE ScriptValue(double value, Allocator* allocator);
		Y_PRIVATE ScriptValue(const StaticString& value, Allocator* allocator);
		Y_PRIVATE ScriptValue(const StaticString& value, Type type, Allocator* allocator);
	};
}

#endif
