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
		std::string _name;
		std::vector<ScriptValue*> _args;
		explicit ScriptCommand(std::string_view name)
			: _name{ name } {}
	};

	class ScriptCodePrivate
	{
	public:
		ScriptCodePrivate(const ScriptCodePrivate& other)
		{
			_commands.reserve(other._commands.size());
			for (const auto& other_command : other._commands)
			{
				auto& command = _commands.emplace_back(other_command._name);
				command._args.reserve(other_command._args.size());
				for (const auto other_arg : other_command._args)
					command._args.emplace_back(new (_temporaries.allocate()) ScriptValue{ *other_arg });
			}
		}

		explicit ScriptCodePrivate(std::string&& text)
		{
			ScriptScanner scanner{ text };
			for (ScriptCommand* command = nullptr;;)
			{
				const auto token = scanner.read();
				if (token.type == ScriptScanner::Token::End)
				{
					if (command && command->_name == "=" && command->_args.size() != 2)
						throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
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
						throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
					}
				}
				else
				{
					switch (token.type)
					{
					case ScriptScanner::Token::Identifier:
						if (command->_name == "=" && command->_args.size() != 1)
							throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
						command->_args.emplace_back(new (_temporaries.allocate()) ScriptValue{ token.string, ScriptValue::Type::Name });
						break;

					case ScriptScanner::Token::Number:
						if (command->_name == "=" && command->_args.size() != 1)
							throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
						command->_args.emplace_back(new (_temporaries.allocate()) ScriptValue{ token.string, ScriptValue::Type::Literal });
						break;

					case ScriptScanner::Token::String:
						if (command->_name == "=" && command->_args.size() != 1)
							throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
						command->_args.emplace_back(new (_temporaries.allocate()) ScriptValue{ token.string, ScriptValue::Type::String });
						break;

					case ScriptScanner::Token::Separator:
						if (command->_name == "=" && command->_args.size() != 2)
							throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
						command = nullptr;
						break;

					case ScriptScanner::Token::Equals:
						if (!command->_args.empty())
							throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
						command->_args.emplace_back(new (_temporaries.allocate()) ScriptValue{ command->_name, ScriptValue::Type::Name });
						command->_name = "=";
						break;

					default:
						throw DataError{ "[", token.line, ":", token.column, "] Unexpected token" };
					}
				}
			}
		}

		ScriptCodePrivate& operator=(const ScriptCodePrivate&) = delete;

	public:
		Pool<ScriptValue> _temporaries{ 32 };
		std::vector<ScriptCommand> _commands;
	};

	ScriptCode::ScriptCode() = default;

	ScriptCode::ScriptCode(const ScriptCode& other)
		: _private{ std::make_unique<ScriptCodePrivate>(*other._private) }
	{
	}

	ScriptCode::ScriptCode(ScriptCode&&) noexcept = default;

	ScriptCode::~ScriptCode() noexcept = default;

	ScriptCode& ScriptCode::operator=(ScriptCode&&) noexcept = default;

	ScriptCode::ScriptCode(std::string&& text)
		: _private{ std::make_unique<ScriptCodePrivate>(std::move(text)) }
	{
	}

	void ScriptCode::execute(ScriptContext& context) const
	{
		if (!_private)
			return;
		std::string result;
		for (const auto& command : _private->_commands)
			if (command._name == "=")
			{
				assert(command._args.size() == 2);
				assert(command._args[0]->type() == ScriptValue::Type::Name);
				if (command._args[1]->type() == ScriptValue::Type::Name)
				{
					const auto value = context.find(command._args[1]->string());
					context.set(command._args[0]->string(), value ? value->string() : std::string{});
				}
				else
					context.set(command._args[0]->string(), command._args[1]->string());
			}
			else if (!context.call(command._name, result, ScriptArgs{ context, command._args }))
				break;
	}
}
