#include <yttrium/script/args.h>

#include <yttrium/script/context.h>
#include <yttrium/script/value.h>

namespace Yttrium
{
	int ScriptArgs::get_int(size_t index, int default_value) const
	{
		return index < _values.size() ? _values[index]->to_int() : default_value;
	}

	const ScriptValue* ScriptArgs::operator[](size_t index) const
	{
		if (index >= _values.size())
			return nullptr;
		const ScriptValue* value = _values[index];
		return value->type() == ScriptValue::Type::Name ? _context.find(value->to_string()) : value;
	}
}
