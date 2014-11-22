#include <yttrium/script/context.h>

#include <yttrium/assert.h>
#include <yttrium/file.h>
#include <yttrium/log.h>

namespace Yttrium
{

ScriptContext::ScriptContext(Allocator *allocator)
	: _allocator(allocator)
	, _parent(nullptr)
	, _values(32, _allocator) // TODO: Get rid of magic numbers.
{
}

ScriptContext::ScriptContext(ScriptContext *parent, Allocator *allocator)
	: _allocator(allocator ? allocator : parent->_allocator)
	, _parent(parent)
	, _values(32, _allocator) // TODO: Get rid of magic numbers.
{
}

ScriptContext::Archive ScriptContext::archive() const
{
	Archive result;

	for (const auto &entity: _entities)
	{
		if (entity.second.archived_value)
		{
			String value(_allocator);

			if (entity.second.archived_value->type() == ScriptValue::String)
			{
				value << '"' << entity.second.archived_value->to_string().escaped("\\\"", '\\') << '"';
			}
			else
			{
				value = entity.second.archived_value->to_string();
			}

			result.emplace(entity.first, value);
		}
	}

	return result;
}

bool ScriptContext::call(const StaticString &name, String *result, const ScriptArgs &args)
{
	if (name.is_empty())
	{
		return false;
	}

	StaticString id = (name[0] == '+' || name[0] == '-' ? StaticString(name.text() + 1, name.size() - 1) : name);

	const auto command = _commands.find(String(id, ByReference(), nullptr));

	if (command == _commands.end())
	{
		Y_LOG(S("[Script.Context] Unknown command \"") << id << '\"');
		return false;
	}

	if (args.size() < command->second.min_args || args.size() > command->second.max_args)
	{
		Y_LOG(S("[Script.Context] Argument number mismatch for command \"") << id << '\"');
		return false;
	}

	command->second.command(name, result, args);

	return true;
}

void ScriptContext::define(const StaticString& name, const Command& command, size_t min_args, size_t max_args)
{
	_commands[String(name, _allocator)] = CommandContext(command, min_args, max_args);
}

bool ScriptContext::execute(const StaticString& text, ScriptCode::ExecutionMode mode)
{
	const ScriptCode& code = ScriptCode(text);
	if (!code)
		return false;
	code.execute(this, mode);
	return true;
}

bool ScriptContext::execute_file(const StaticString& name)
{
	const ScriptCode& code = ScriptCode::load(name);
	if (!code)
		return false;
	code.execute(this, ScriptCode::Do);
	return true;
}

ScriptValue* ScriptContext::find(const StaticString& name) const
{
	const auto i = _entities.find(String(name, ByReference(), nullptr));
	if (i != _entities.end())
		return i->second.value;

	if (_parent)
		return _parent->find(name);

	return nullptr;
}

ScriptContext& ScriptContext::root()
{
	return _parent ? _parent->root() : *this;
}

const ScriptValue* ScriptContext::set(const StaticString &name, int32_t value, unsigned flags)
{
	auto i = _entities.find(String(name, ByReference(), nullptr));

	if (i == _entities.end())
	{
		i = _entities.emplace(String(name, _allocator),
			Entity(new(_values.allocate()) ScriptValue(value, _allocator))).first;
	}
	else if (!(flags & ScriptValue::Default))
	{
		*i->second.value = value;
	}

	if (flags & ScriptValue::Archived)
	{
		if (!i->second.archived_value)
		{
			i->second.archived_value = new(_values.allocate()) ScriptValue(value, _allocator);
		}
		else if (!(flags & ScriptValue::Default))
		{
			*i->second.archived_value = value;
		}
	}

	return i->second.value;
}

const ScriptValue* ScriptContext::set(const StaticString& name, double value, unsigned flags)
{
	auto i = _entities.find(String(name, ByReference(), nullptr));

	if (i == _entities.end())
	{
		i = _entities.emplace(String(name, _allocator),
			Entity(new(_values.allocate()) ScriptValue(value, _allocator))).first;
	}
	else if (!(flags & ScriptValue::Default))
	{
		*i->second.value = value;
	}

	if (flags & ScriptValue::Archived)
	{
		if (!i->second.archived_value)
		{
			i->second.archived_value = new(_values.allocate()) ScriptValue(value, _allocator);
		}
		else if (!(flags & ScriptValue::Default))
		{
			*i->second.archived_value = value;
		}
	}

	return i->second.value;
}

const ScriptValue* ScriptContext::set(const StaticString& name, const StaticString& value, unsigned flags)
{
	auto i = _entities.find(String(name, ByReference(), nullptr));

	if (i == _entities.end())
	{
		i = _entities.emplace(String(name, _allocator),
			Entity(new(_values.allocate()) ScriptValue(value, _allocator))).first;
	}
	else if (!(flags & ScriptValue::Default))
	{
		*i->second.value = value;
	}

	if (flags & ScriptValue::Archived)
	{
		if (!i->second.archived_value)
		{
			i->second.archived_value = new(_values.allocate()) ScriptValue(value, _allocator);
		}
		else if (!(flags & ScriptValue::Default))
		{
			*i->second.archived_value = value;
		}
	}

	return i->second.value;
}

void ScriptContext::substitute(String& target, const StaticString& source) const
{
	Y_ASSERT(target.text() != source.text());

	target.clear();
	for (auto left = source.text(), right = left, end = left + source.size(); ; )
	{
		while (right != end && *right != '{')
			++right;

		target.append(left, right - left);

		if (right == end)
			break;

		left = ++right;

		while (right != end && *right != '}')
			++right;

		if (right == end)
			break;

		ScriptValue* value = find(StaticString(left, right - left));
		if (value)
			target.append(value->to_string());

		left = ++right;
	}
}

void ScriptContext::unset(const StaticString& name)
{
	const auto i = _entities.find(String(name, ByReference()));
	if (i == _entities.end())
		return;

	_values.deallocate(i->second.value);
	if (i->second.archived_value)
		_values.deallocate(i->second.archived_value);
	_entities.erase(i);
}

void ScriptContext::undefine(const StaticString& name)
{
	_commands.erase(String(name, ByReference()));
}

} // namespace Yttrium
