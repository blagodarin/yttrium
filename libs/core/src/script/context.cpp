//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <yttrium/script/context.h>

#include <yttrium/logger.h>
#include <yttrium/memory/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/utils/string.h>

#include <algorithm>
#include <unordered_map>

namespace Yt
{
	struct ScriptCommandContext
	{
		ScriptContext::Command _command;
		size_t _min_args = 0;
		size_t _max_args = 0;

		ScriptCommandContext() = default;
		ScriptCommandContext(const ScriptContext::Command& command, size_t min_args, size_t max_args)
			: _command{ command }, _min_args{ min_args }, _max_args{ max_args } {}
	};

	class ScriptContextPrivate
	{
	public:
		explicit ScriptContextPrivate(ScriptContext* parent)
			: _parent{ parent } {}

	public:
		ScriptContext* const _parent;
		Pool<ScriptValue> _value_pool{ 32 };
		std::unordered_map<std::string, ScriptValue*> _values;
		std::unordered_map<std::string, ScriptCommandContext> _commands;
	};

	ScriptContext::ScriptContext(ScriptContext* parent)
		: _private{ std::make_unique<ScriptContextPrivate>(parent) }
	{
	}

	ScriptContext::~ScriptContext() = default;

	bool ScriptContext::call(const std::string& name, std::string& result, const ScriptArgs& args)
	{
		if (name.empty())
		{
			Logger::log("Invalid command \"\"");
			return false;
		}

		const auto command = _private->_commands.find(name);
		if (command == _private->_commands.end())
		{
			Logger::log("Unknown command \"", name, '"');
			return false;
		}

		if (args.size() < command->second._min_args || args.size() > command->second._max_args)
		{
			Logger::log("Argument number mismatch for \"", name, "\": ", args.size(), " instead of ", command->second._min_args, "-", command->second._max_args);
			return false;
		}

		command->second._command({ *this, name, result, args });
		return true;
	}

	bool ScriptContext::call(const std::string& name, const std::vector<std::string>& arg_strings)
	{
		const auto command = _private->_commands.find(name);
		if (command == _private->_commands.end())
		{
			Logger::log("Unknown command \"", name, '"');
			return false;
		}

		if (arg_strings.size() < command->second._min_args || arg_strings.size() > command->second._max_args)
		{
			Logger::log("Argument number mismatch for \"", name, "\": ", arg_strings.size(), " instead of ", command->second._min_args, "-", command->second._max_args);
			return false;
		}

		Pool<ScriptValue> arg_pool{ arg_strings.size() };
		std::vector<ScriptValue*> arg_values;
		arg_values.reserve(arg_strings.size());
		std::transform(arg_strings.begin(), arg_strings.end(), std::back_inserter(arg_values),
			[&arg_pool](const std::string& arg_string) { return new (arg_pool.allocate()) ScriptValue{ arg_string }; });
		std::string result;
		command->second._command({ *this, name, result, ScriptArgs{ *this, arg_values } });
		return true;
	}

	void ScriptContext::define(const std::string& name, size_t min_args, size_t max_args, const Command& command)
	{
		_private->_commands[name] = ScriptCommandContext{ command, min_args, max_args };
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
			_private->_values.emplace(name, new (_private->_value_pool.allocate()) ScriptValue{ value });
		else
			*i->second = value;
	}

	void ScriptContext::set(const std::string& name, std::string_view value)
	{
		const auto i = _private->_values.find(name);
		if (i == _private->_values.end())
			_private->_values.emplace(name, new (_private->_value_pool.allocate()) ScriptValue{ value });
		else
			*i->second = value;
	}

	void ScriptContext::substitute(std::string& target, std::string_view source) const
	{
		target.clear();
		for (auto left = source.data(), right = left, end = left + source.size();;)
		{
			right = std::find(right, end, '{');
			append_to(target, std::string_view{ left, static_cast<size_t>(right - left) });
			if (right == end)
				break;
			left = ++right;
			right = std::find(right, end, '}');
			if (right == end)
				break;
			const auto value = find({ left, static_cast<size_t>(right - left) });
			if (value)
				append_to(target, value->string());
			left = ++right;
		}
	}
}
