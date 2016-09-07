#include <yttrium/script/args.h>

#include <yttrium/script/context.h>
#include <yttrium/script/value.h>

namespace Yttrium
{
	int ScriptArgs::get_int(size_t index, int default_value) const
	{
		return index < _values.size() ? _values[index]->to_int() : default_value;
	}

	String ScriptArgs::string(size_t index, NameResolution resolution) const
	{
		String result(&_context._private.allocator());

		if (index < _values.size())
		{
			const ScriptValue* value = _values[index];

			if (resolution == Resolve && value->type() == ScriptValue::Type::Name)
				value = _context.find(value->to_string());

			if (value)
				result = value->to_string();
		}

		return result;
	}

	const ScriptValue* ScriptArgs::value(size_t index) const
	{
		return index < _values.size() ? _values[index] : nullptr;
	}
}
