/// \file
/// \brief Script value.

#ifndef _include_yttrium_script_value_h_
#define _include_yttrium_script_value_h_

#include <yttrium/string_utils.h>

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
		int to_int() const { return strings::to_int(_value); }

		///
		Type type() const { return _type; }

		///
		const std::string& string() const noexcept { return _value; }

	public:
		///
		ScriptValue& operator=(int);

		///
		ScriptValue& operator=(std::string_view);

	private:
		Type _type;
		std::string _value;

	private:
		Y_PRIVATE explicit ScriptValue(int);
		Y_PRIVATE explicit ScriptValue(double);
		Y_PRIVATE explicit ScriptValue(std::string_view, Type = Type::String);
	};
}

#endif
