#include <yttrium/script/context.h>

#include <yttrium/memory/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/string.h>
#include <yttrium/string_utils.h>

#include <cassert>
#include <iostream>
#include <map>

namespace Yttrium
{
	struct ScriptCommandContext
	{
		ScriptContext::Command command;
		size_t min_args = 0;
		size_t max_args = 0;

		ScriptCommandContext() = default;

		ScriptCommandContext(ScriptContext::Command command, size_t min_args, size_t max_args)
			: command(command)
			, min_args(min_args)
			, max_args(max_args)
		{
		}
	};

	class ScriptContextPrivate
	{
	public:
		ScriptContextPrivate(Allocator& allocator, ScriptContext* parent)
			: _allocator(allocator)
			, _parent(parent)
		{
		}

	public:
		Allocator& _allocator;
		ScriptContext* const _parent;
		Pool<ScriptValue> _value_pool{ 32, _allocator };
		std::map<String, ScriptValue*> _values;
		std::map<String, ScriptCommandContext> _commands;
	};

	ScriptContext::ScriptContext(Allocator* allocator)
		: _private(std::make_unique<ScriptContextPrivate>(*allocator, nullptr))
	{
	}

	ScriptContext::ScriptContext(ScriptContext* parent, Allocator* allocator)
		: _private(std::make_unique<ScriptContextPrivate>(*allocator, parent))
	{
	}

	Allocator& ScriptContext::allocator() const noexcept
	{
		return _private->_allocator;
	}

	bool ScriptContext::call(const StaticString& name, String* result, const ScriptArgs& args)
	{
		if (name.is_empty())
		{
			std::cerr << "Invalid command \"\"\n";
			return false;
		}

		const auto id = (name[0] == '+' || name[0] == '-' ? StaticString(name.text() + 1, name.size() - 1) : name);

		const auto command = _private->_commands.find(String(id, ByReference(), nullptr));
		if (command == _private->_commands.end())
		{
			std::cerr << "Unknown command \"" << id << "\"\n";
			return false;
		}

		if (args.size() < command->second.min_args || args.size() > command->second.max_args)
		{
			std::cerr << "Argument number mismatch for \"" << id << "\": "
				<< args.size() << " instead of " << command->second.min_args << "-" << command->second.max_args << "\n";
			return false;
		}

		command->second.command(ScriptCall(*this, name, args, *result));
		return true;
	}

	void ScriptContext::define(const StaticString& name, size_t min_args, size_t max_args, const Command& command)
	{
		_private->_commands[String(name, &_private->_allocator)] = ScriptCommandContext(command, min_args, max_args);
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
			i = _private->_values.emplace(String(name, &_private->_allocator), new(_private->_value_pool.allocate()) ScriptValue(value)).first;
		else
			*i->second = value;
		return i->second;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, double value)
	{
		auto i = _private->_values.find(String(name, ByReference(), nullptr));
		if (i == _private->_values.end())
			i = _private->_values.emplace(String(name, &_private->_allocator), new(_private->_value_pool.allocate()) ScriptValue(value)).first;
		else
			*i->second = value;
		return i->second;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, const StaticString& value)
	{
		auto i = _private->_values.find(String(name, ByReference(), nullptr));
		if (i == _private->_values.end())
			i = _private->_values.emplace(String(name, &_private->_allocator), new(_private->_value_pool.allocate()) ScriptValue(value)).first;
		else
			*i->second = value;
		return i->second;
	}

	void ScriptContext::substitute(std::string& target, const StaticString& source) const
	{
		target.clear();
		for (auto left = source.text(), right = left, end = left + source.size(); ; )
		{
			while (right != end && *right != '{')
				++right;

			append_to(target, StaticString(left, right - left));

			if (right == end)
				break;

			left = ++right;

			while (right != end && *right != '}')
				++right;

			if (right == end)
				break;

			const auto value = find(StaticString(left, right - left));
			if (value)
				append_to(target, value->to_string());

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

	ScriptContext::~ScriptContext() = default;
}
