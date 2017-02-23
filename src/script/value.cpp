#include <yttrium/script/value.h>

#include <yttrium/string_utils.h>

namespace Yttrium
{
	ScriptValue& ScriptValue::operator=(int value)
	{
		_type = Type::Literal;
		_value.clear();
		append_to(_value, value);
		return *this;
	}

	ScriptValue& ScriptValue::operator=(const std::string& value)
	{
		_type = Type::String;
		_value = value;
		return *this;
	}

	ScriptValue::ScriptValue(int value)
		: _type(Type::Literal)
		, _value(make_string(value))
	{
	}

	ScriptValue::ScriptValue(const StaticString& value, Type type)
		: _type(type)
		, _value(make_string(value))
	{
	}
}
