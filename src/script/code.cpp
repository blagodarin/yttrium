#include <yttrium/script/code.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include "scanner.h"

#include <cassert>

namespace Yttrium
{
	struct ScriptCommand
	{
		std::string name;
		std::vector<ScriptValue*> args;

		ScriptCommand(const StaticString& name)
			: name(name.text(), name.size())
		{
		}
	};

	class ScriptCodePrivate
	{
	public:
		ScriptCodePrivate(std::string&& text, Allocator& allocator)
			: _temporaries{ 32, allocator }
		{
			ScriptScanner scanner(text);
			for (ScriptCommand* command = nullptr;;)
			{
				const auto token = scanner.read();
				if (token.type == ScriptScanner::Token::End)
				{
					if (command && command->name == "=" && command->args.size() != 2)
						throw DataError("[", token.line, ":", token.column, "] Unexpected token");
					break;
				}
				if (!command)
				{
					switch (token.type)
					{
					case ScriptScanner::Token::Identifier:
						_commands.emplace_back(token.string);
						command = &_commands.back();
						break;

					case ScriptScanner::Token::Separator:
						break;

					default:
						throw DataError("[", token.line, ":", token.column, "] Unexpected token");
					}
				}
				else
				{
					switch (token.type)
					{
					case ScriptScanner::Token::Identifier:
						if (command->name == "=" && command->args.size() != 1)
							throw DataError("[", token.line, ":", token.column, "] Unexpected token");
						command->args.emplace_back(new(_temporaries.allocate()) ScriptValue(token.string, ScriptValue::Type::Name));
						break;

					case ScriptScanner::Token::Number:
						if (command->name == "=" && command->args.size() != 1)
							throw DataError("[", token.line, ":", token.column, "] Unexpected token");
						command->args.emplace_back(new(_temporaries.allocate()) ScriptValue(token.string, ScriptValue::Type::Literal));
						break;

					case ScriptScanner::Token::String:
						if (command->name == "=" && command->args.size() != 1)
							throw DataError("[", token.line, ":", token.column, "] Unexpected token");
						command->args.emplace_back(new(_temporaries.allocate()) ScriptValue(token.string, ScriptValue::Type::String));
						break;

					case ScriptScanner::Token::Separator:
						if (command->name == "=" && command->args.size() != 2)
							throw DataError("[", token.line, ":", token.column, "] Unexpected token");
						command = nullptr;
						break;

					case ScriptScanner::Token::Equals:
						if (!command->args.empty())
							throw DataError("[", token.line, ":", token.column, "] Unexpected token");
						command->args.emplace_back(new(_temporaries.allocate()) ScriptValue(StaticString{ command->name }, ScriptValue::Type::Name));
						command->name = "=";
						break;

					default:
						throw DataError("[", token.line, ":", token.column, "] Unexpected token");
					}
				}
			}
		}

	public:
		Pool<ScriptValue> _temporaries;
		std::vector<ScriptCommand> _commands;
	};

	ScriptCode::ScriptCode() = default;
	ScriptCode::~ScriptCode() = default;

	ScriptCode::ScriptCode(std::string&& text, Allocator& allocator)
		: _private(std::make_unique<ScriptCodePrivate>(std::move(text), allocator))
	{
	}

	void ScriptCode::execute(ScriptContext& context) const
	{
		if (!_private)
			return;
		std::string result;
		for (const auto& command : _private->_commands)
			if (command.name == "=")
			{
				assert(command.args.size() == 2);
				assert(command.args[0]->type() == ScriptValue::Type::Name);
				if (command.args[1]->type() == ScriptValue::Type::Name)
				{
					const auto value = context.find(command.args[1]->to_string());
					context.set(command.args[0]->to_string(), value ? value->to_string() : StaticString());
				}
				else
					context.set(command.args[0]->to_string(), command.args[1]->to_string());
			}
			else if (!context.call(command.name, result, ScriptArgs(context, command.args)))
				break;
	}

	ScriptCode::ScriptCode(ScriptCode&&) noexcept = default;
	ScriptCode& ScriptCode::operator=(ScriptCode&&) noexcept = default;
}
