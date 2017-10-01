#include <yttrium/script/value.h>

namespace Yttrium
{
	ScriptValue& ScriptValue::operator=(int value)
	{
		_type = Type::Literal;
		_value.clear();
		append_to(_value, value);
		return *this;
	}

	ScriptValue& ScriptValue::operator=(std::string_view value)
	{
		_type = Type::String;
		_value = value;
		return *this;
	}

	ScriptValue::ScriptValue(int value)
		: _type{Type::Literal}
		, _value{make_string(value)}
	{
	}

	ScriptValue::ScriptValue(std::string_view value, Type type)
		: _type{type}
		, _value{value}
	{
	}
}
