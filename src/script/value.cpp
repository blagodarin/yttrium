#include <Yttrium/script/value.h>

namespace Yttrium
{

ScriptValue &ScriptValue::operator =(Integer value)
{
	_type = Literal;
	_value.clear().append_dec(value);
	return *this;
}

ScriptValue &ScriptValue::operator =(Real value)
{
	_type = Literal;
	_value.clear().append_dec(value);
	return *this;
}

ScriptValue &ScriptValue::operator =(const StaticString &value)
{
	_type = String;
	_value.clear().append(value);
	return *this;
}

ScriptValue::ScriptValue(Integer value, Allocator *allocator)
	: _type(Literal)
	, _value(allocator)
{
	_value.append_dec(value);
}

ScriptValue::ScriptValue(Real value, Allocator *allocator)
	: _type(Literal)
	, _value(allocator)
{
	_value.append_dec(value);
}

ScriptValue::ScriptValue(const StaticString &value, Allocator *allocator)
	: _type(String)
	, _value(value, allocator)
{
}

ScriptValue::ScriptValue(const StaticString &value, Type type, Allocator *allocator)
	: _type(type)
	, _value(value, allocator)
{
}

} // namespace Yttrium
