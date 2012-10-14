#include <Yttrium/script/context.h>

#include <Yttrium/assert.h>

#include "logging.h"
#include "parser.h"

namespace Yttrium
{

ScriptContext::ScriptContext(Allocator *allocator)
	: _allocator(allocator)
	, _parent(nullptr)
{
}

ScriptContext::ScriptContext(ScriptContext *parent, Allocator *allocator)
	: _allocator(allocator ? allocator : parent->_allocator)
	, _parent(parent)
{
}

ScriptContext::Archive ScriptContext::archive() const
{
	Archive result;

	for (Entities::const_iterator i = _entities.begin(); i != _entities.end(); ++i)
	{
		if (i->second.archived_value)
		{
			String value(_allocator);

			if (i->second.archived_value->type() == ScriptValue::String)
			{
				value << '"' << i->second.archived_value->string().escaped("\\\"", '\\') << '"';
			}
			else
			{
				value = i->second.archived_value->string();
			}

			result[i->first] = value;
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

	Y_LOG_TRACE(S("[Script.Context] Calling \"") << name << S("\"..."));

	StaticString id = (name[0] == '+' || name[0] == '-' ? StaticString(name.text() + 1, name.size() - 1) : name);

	Commands::iterator command = _commands.find(String(id, ByReference(), nullptr));

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

void ScriptContext::define(const StaticString &name, const Command &command, size_t min_args, size_t max_args)
{
	_commands[String(name, _allocator)] = CommandContext(command, min_args, max_args);
}

bool ScriptContext::execute(const StaticString &text, ExecutionMode mode)
{
	if (text.is_empty())
	{
		return true; // NOTE: Why true?
	}

	ScriptParser parser(this, _allocator);

	return parser.parse(text) && parser.execute(mode);
}

bool ScriptContext::execute_file(const StaticString &name)
{
	String buffer(_allocator);

	return File(name, _allocator).read_all(&buffer) && execute(buffer, ExecutionMode::Do);
}

ScriptValue *ScriptContext::find(const StaticString &name) const
{
	Entities::const_iterator i = _entities.find(String(name, ByReference(), nullptr));

	if (i != _entities.end())
	{
		return i->second.value;
	}

	if (_parent)
	{
		return _parent->find(name);
	}

	return nullptr;
}

ScriptContext *ScriptContext::root()
{
	return (_parent ? _parent->root() : this);
}

const ScriptValue *ScriptContext::set(const StaticString &name, Integer value, ScriptValue::Flags flags)
{
	Entities::iterator i = _entities.find(String(name, ByReference(), nullptr));

	if (i != _entities.end() && !(flags & ScriptValue::Default))
	{
		*i->second.value = value;
	}
	else
	{
		i = _entities.insert(Entities::value_type(
			String(name, _allocator),
			Entity(new(_values.allocate()) ScriptValue(value, _allocator)))).first;
	}

	if (flags & ScriptValue::Archived)
	{
		if (i->second.archived_value && !(flags & ScriptValue::Default))
		{
			*i->second.archived_value = value;
		}
		else
		{
			i->second.archived_value = new(_values.allocate()) ScriptValue(value, _allocator);
		}
	}

	return i->second.value;
}

const ScriptValue *ScriptContext::set(const StaticString &name, Real value, ScriptValue::Flags flags)
{
	Entities::iterator i = _entities.find(String(name, ByReference(), nullptr));

	if (i != _entities.end() && !(flags & ScriptValue::Default))
	{
		*i->second.value = value;
	}
	else
	{
		i = _entities.insert(Entities::value_type(
			String(name, _allocator),
			Entity(new(_values.allocate()) ScriptValue(value, _allocator)))).first;
	}

	if (flags & ScriptValue::Archived)
	{
		if (i->second.archived_value && !(flags & ScriptValue::Default))
		{
			*i->second.archived_value = value;
		}
		else
		{
			i->second.archived_value = new(_values.allocate()) ScriptValue(value, _allocator);
		}
	}

	return i->second.value;
}

const ScriptValue *ScriptContext::set(const StaticString &name, const StaticString &value, ScriptValue::Flags flags)
{
	Entities::iterator i = _entities.find(String(name, ByReference(), nullptr));

	if (i != _entities.end() && !(flags & ScriptValue::Default))
	{
		*i->second.value = value;
	}
	else
	{
		i = _entities.insert(Entities::value_type(
			String(name, _allocator),
			Entity(new(_values.allocate()) ScriptValue(value, _allocator)))).first;
	}

	if (flags & ScriptValue::Archived)
	{
		if (i->second.archived_value && !(flags & ScriptValue::Default))
		{
			*i->second.archived_value = value;
		}
		else
		{
			i->second.archived_value = new(_values.allocate()) ScriptValue(value, _allocator);
		}
	}

	return i->second.value;
}

void ScriptContext::substitute(String *target, const StaticString &source) const
{
	Y_ASSERT(target->text() != source.text());

	for (const char *left = source.text(), *right = left, *end = left + source.size(); ; )
	{
		while (right != end && *right != '{')
		{
			++right;
		}

		target->append(left, right - left);

		if (right == end)
		{
			break;
		}

		left = ++right;

		while (right != end && *right != '}')
		{
			++right;
		}

		if (right == end)
		{
			break;
		}

		ScriptValue *value = find(StaticString(left, right - left));

		if (value)
		{
			target->append(value->string());
		}

		left = ++right;
	}
}

void ScriptContext::unset(const StaticString &name)
{
	Entities::iterator i = _entities.find(String(name, ByReference()));

	if (i != _entities.end())
	{
		_values.deallocate(i->second.value);

		if (i->second.archived_value)
		{
			_values.deallocate(i->second.archived_value);
		}

		_entities.erase(i);
	}
}

void ScriptContext::undefine(const StaticString &name)
{
	_commands.erase(String(name, ByReference()));
}

} // namespace Yttrium
