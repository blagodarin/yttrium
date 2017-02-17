/// \file
/// \brief Script value.

#ifndef _include_yttrium_script_value_h_
#define _include_yttrium_script_value_h_

#include <yttrium/static_string.h>

namespace Yttrium
{
	class ScriptArgs;
	class ScriptCodePrivate;
	class ScriptContext;
	class ScriptParser;

	/// Script value data.
	class Y_API ScriptValue
	{
		friend ScriptArgs;
		friend ScriptCodePrivate;
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
		int to_int() const { return StaticString{ _value }.to_int(); }

		///
		double to_double() const { return StaticString{ _value }.to_double(); }

		///
		StaticString to_string() const { return StaticString{ _value }; }

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
		std::string _value;

	private:
		Y_PRIVATE ScriptValue(int);
		Y_PRIVATE ScriptValue(double);
		Y_PRIVATE ScriptValue(const StaticString&, Type = Type::String);
	};
}

#endif
