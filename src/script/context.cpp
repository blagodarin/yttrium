#include <yttrium/script/context.h>

#include <yttrium/assert.h>
#include <yttrium/file.h>
#include <yttrium/log.h>

namespace Yttrium
{
	ScriptContext::ScriptContext(Allocator* allocator)
		: _allocator(allocator)
		, _parent(nullptr)
		, _value_pool(32, _allocator) // TODO: Get rid of magic numbers.
	{
	}

	ScriptContext::ScriptContext(ScriptContext* parent, Allocator* allocator)
		: _allocator(allocator ? allocator : parent->_allocator)
		, _parent(parent)
		, _value_pool(32, _allocator) // TODO: Get rid of magic numbers.
	{
	}

	bool ScriptContext::call(const StaticString& name, String* result, const ScriptArgs& args)
	{
		if (name.is_empty())
			return false;

		StaticString id = (name[0] == '+' || name[0] == '-' ? StaticString(name.text() + 1, name.size() - 1) : name);

		const auto command = _commands.find(String(id, ByReference(), nullptr));

		if (command == _commands.end())
		{
			Log() << S("[Script.Context] Unknown command \"") << id << '\"';
			return false;
		}

		if (args.size() < command->second.min_args || args.size() > command->second.max_args)
		{
			Log() << S("[Script.Context] Argument number mismatch for command \"") << id << '\"';
			return false;
		}

		command->second.command(ScriptCall(*this, name, args, *result));

		return true;
	}

	void ScriptContext::define(const StaticString& name, size_t min_args, size_t max_args, const Command& command)
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
		const auto i = _values.find(String(name, ByReference(), nullptr));
		if (i != _values.end())
			return i->second;
		if (_parent)
			return _parent->find(name);
		return nullptr;
	}

	ScriptContext& ScriptContext::root()
	{
		return _parent ? _parent->root() : *this;
	}

	// TODO: Merge the following three functions.

	const ScriptValue* ScriptContext::set(const StaticString& name, int value)
	{
		auto i = _values.find(String(name, ByReference(), nullptr));
		if (i == _values.end())
			i = _values.emplace(String(name, _allocator), new(_value_pool.allocate()) ScriptValue(value, _allocator)).first;
		else
			*i->second = value;
		return i->second;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, double value)
	{
		auto i = _values.find(String(name, ByReference(), nullptr));
		if (i == _values.end())
			i = _values.emplace(String(name, _allocator), new(_value_pool.allocate()) ScriptValue(value, _allocator)).first;
		else
			*i->second = value;
		return i->second;
	}

	const ScriptValue* ScriptContext::set(const StaticString& name, const StaticString& value)
	{
		auto i = _values.find(String(name, ByReference(), nullptr));
		if (i == _values.end())
			i = _values.emplace(String(name, _allocator), new(_value_pool.allocate()) ScriptValue(value, _allocator)).first;
		else
			*i->second = value;
		return i->second;
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
		const auto i = _values.find(String(name, ByReference()));
		if (i == _values.end())
			return;
		_value_pool.deallocate(i->second);
		_values.erase(i);
	}

	void ScriptContext::undefine(const StaticString& name)
	{
		_commands.erase(String(name, ByReference()));
	}
}
