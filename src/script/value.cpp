#include <yttrium/script/value.h>

namespace Yttrium
{
	ScriptValue& ScriptValue::operator=(int value)
	{
		_type = Type::Literal;
		_value.clear().append_dec(value);
		return *this;
	}

	ScriptValue& ScriptValue::operator=(double value)
	{
		_type = Type::Literal;
		_value.clear().append_dec(value);
		return *this;
	}

	ScriptValue& ScriptValue::operator=(const StaticString& value)
	{
		_type = Type::String;
		_value.set(value);
		return *this;
	}

	ScriptValue::ScriptValue(int value, Allocator* allocator)
		: _type(Type::Literal)
		, _value(allocator)
	{
		_value.append_dec(value);
	}

	ScriptValue::ScriptValue(double value, Allocator* allocator)
		: _type(Type::Literal)
		, _value(allocator)
	{
		_value.append_dec(value);
	}

	ScriptValue::ScriptValue(const StaticString& value, Allocator* allocator)
		: _type(Type::String)
		, _value(value, allocator)
	{
	}

	ScriptValue::ScriptValue(const StaticString& value, Type type, Allocator* allocator)
		: _type(type)
		, _value(value, allocator)
	{
	}
}
