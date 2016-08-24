#include "context.h"

#include <yttrium/file.h>
#include <yttrium/log.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>

#include <cassert>

namespace Yttrium
{
	Y_IMPLEMENT_UNIQUE(ScriptContext);

	ScriptContext::ScriptContext(Allocator* allocator)
		: _private(make_raw<Private>(*allocator, nullptr, *allocator))
	{
	}

	ScriptContext::ScriptContext(ScriptContext* parent, Allocator* allocator)
		: _private(make_raw<Private>(*allocator, parent, *allocator))
	{
	}

	Allocator& ScriptContext::allocator() const
	{
		return *_private->_allocator;
	}

	bool ScriptContext::call(const StaticString& name, String* result, const ScriptArgs& args)
	{
		if (name.is_empty())
			return false;

		StaticString id = (name[0] == '+' || name[0] == '-' ? StaticString(name.text() + 1, name.size() - 1) : name);

		const auto command = _private->_commands.find(String(id, ByReference(), nullptr));

		if (command == _private->_commands.end())
		{
			Log() << "[Script.Context] Unknown command \""_s << id << '\"';
			return false;
		}

		if (args.size() < command->second.min_args || args.size() > command->second.max_args)
		{
			Log() << "[Script.Context] Argument number mismatch for command \""_s << id << '\"';
			return false;
		}

		command->second.command(ScriptCall(*this, name, args, *result));

		return true;
	}

	void ScriptContext::define(const StaticString& name, size_t min_args, size_t max_args, const Command& command)
	{
		_private->_commands[String(name, _private->_allocator)] = Private::CommandContext(command, min_args, max_args);
	}

	ScriptValue* ScriptContext::find(const StaticString& name) const
	{
		const auto i = _private->_values.find(String(name, ByReference(), nullptr));
		if (i != _private->_values.end())
			return i->second;
		if (_private->_parent)
			return _private->_parent->find(name);
		return nullptr;
	}

	int ScriptContext::get_int(const StaticString& name, int default_value)
	{
		const auto value = find(name);
		return value ? value->to_int() : default_value;
	}

	ScriptContext& ScriptContext::root()
	{
		return _private->_parent ? _private->_parent->root() : *this;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, int value)
	{
		auto i = _private->_values.find(String(name, ByReference(), nullptr));
		if (i == _private->_values.end())
			i = _private->_values.emplace(String(name, _private->_allocator),
				new(_private->_value_pool.allocate()) ScriptValue(value, _private->_allocator)).first;
		else
			*i->second = value;
		return i->second;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, double value)
	{
		auto i = _private->_values.find(String(name, ByReference(), nullptr));
		if (i == _private->_values.end())
			i = _private->_values.emplace(String(name, _private->_allocator),
				new(_private->_value_pool.allocate()) ScriptValue(value, _private->_allocator)).first;
		else
			*i->second = value;
		return i->second;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, const StaticString& value)
	{
		auto i = _private->_values.find(String(name, ByReference(), nullptr));
		if (i == _private->_values.end())
			i = _private->_values.emplace(String(name, _private->_allocator),
				new(_private->_value_pool.allocate()) ScriptValue(value, _private->_allocator)).first;
		else
			*i->second = value;
		return i->second;
	}

	void ScriptContext::substitute(String& target, const StaticString& source) const
	{
		assert(target.text() != source.text());

		target.clear();
		for (auto left = source.text(), right = left, end = left + source.size(); ; )
		{
			while (right != end && *right != '{')
				++right;

			target << StaticString(left, right - left);

			if (right == end)
				break;

			left = ++right;

			while (right != end && *right != '}')
				++right;

			if (right == end)
				break;

			ScriptValue* value = find(StaticString(left, right - left));
			if (value)
				target << value->to_string();

			left = ++right;
		}
	}

	void ScriptContext::unset(const StaticString& name)
	{
		const auto i = _private->_values.find(String(name, ByReference()));
		if (i == _private->_values.end())
			return;
		_private->_value_pool.deallocate(i->second);
		_private->_values.erase(i);
	}

	void ScriptContext::undefine(const StaticString& name)
	{
		_private->_commands.erase(String(name, ByReference()));
	}
}
