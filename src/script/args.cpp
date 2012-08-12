#include <Yttrium/script/args.h>

#include <Yttrium/script/context.h>

namespace Yttrium
{

String ScriptArgs::string(size_t index, NameResolution resolution) const
{
	String result(_context->_allocator);

	if (index < _values.size())
	{
		const ScriptValue *value = _values[index];

		if (resolution == Resolve && value->type() == ScriptValue::Name)
		{
			value  = _context->find(value->string());
		}

		if (value)
		{
			result = value->string();
		}
	}

	return result;
}

const ScriptValue *ScriptArgs::value(size_t index) const
{
	return index <= _values.size() ? _values[index] : nullptr;
}

} // namespace Yttrium
