#include <yttrium/script/context.h>

#include <yttrium/memory/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/string_utils.h>

#include <cassert>
#include <iostream>
#include <unordered_map>

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
		std::unordered_map<std::string, ScriptValue*> _values;
		std::unordered_map<std::string, ScriptCommandContext> _commands;
	};

	ScriptContext::ScriptContext(Allocator* allocator)
		: _private(std::make_unique<ScriptContextPrivate>(*allocator, nullptr))
	{
	}

	ScriptContext::ScriptContext(ScriptContext* parent, Allocator* allocator)
		: _private(std::make_unique<ScriptContextPrivate>(*allocator, parent))
	{
	}

	ScriptContext::~ScriptContext() = default;

	Allocator& ScriptContext::allocator() const noexcept
	{
		return _private->_allocator;
	}

	bool ScriptContext::call(const std::string& name, std::string& result, const ScriptArgs& args)
	{
		if (name.empty())
		{
			std::cerr << "Invalid command \"\"\n";
			return false;
		}

		const auto command = _private->_commands.find(name);
		if (command == _private->_commands.end())
		{
			std::cerr << "Unknown command \"" << name << "\"\n";
			return false;
		}

		if (args.size() < command->second.min_args || args.size() > command->second.max_args)
		{
			std::cerr << "Argument number mismatch for \"" << name << "\": "
				<< args.size() << " instead of " << command->second.min_args << "-" << command->second.max_args << "\n";
			return false;
		}

		command->second.command({ *this, name, result, args });
		return true;
	}

	void ScriptContext::define(const std::string& name, size_t min_args, size_t max_args, const Command& command)
	{
		_private->_commands[name] = ScriptCommandContext(command, min_args, max_args);
	}

	ScriptValue* ScriptContext::find(const std::string& name) const
	{
		const auto i = _private->_values.find(name);
		if (i != _private->_values.end())
			return i->second;
		if (_private->_parent)
			return _private->_parent->find(name);
		return nullptr;
	}

	int ScriptContext::get_int(const std::string& name, int default_value)
	{
		const auto value = find(name);
		return value ? value->to_int() : default_value;
	}

	ScriptContext& ScriptContext::root()
	{
		return _private->_parent ? _private->_parent->root() : *this;
	}

	void ScriptContext::set(const std::string& name, int value)
	{
		const auto i = _private->_values.find(name);
		if (i == _private->_values.end())
			_private->_values.emplace(name, new(_private->_value_pool.allocate()) ScriptValue(value));
		else
			*i->second = value;
	}

	void ScriptContext::set(const std::string& name, const std::string& value)
	{
		const auto i = _private->_values.find(name);
		if (i == _private->_values.end())
			_private->_values.emplace(name, new(_private->_value_pool.allocate()) ScriptValue(StaticString{ value }));
		else
			*i->second = value;
	}

	void ScriptContext::substitute(std::string& target, const std::string& source) const
	{
		target.clear();
		for (auto left = source.c_str(), right = left, end = left + source.size(); ; )
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

			const auto value = find({ left, static_cast<size_t>(right - left) });
			if (value)
				append_to(target, value->string());

			left = ++right;
		}
	}
}
