#include <yttrium/script/code.h>

#include <yttrium/exceptions.h>
#include <yttrium/memory/pool.h>
#include <yttrium/memory/temporary_allocator.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include <yttrium/storage/reader.h>
#include "scanner.h"

#include <cassert>

namespace Yttrium
{
	struct ScriptCommand
	{
		String name;
		StdVector<ScriptValue*> args;

		ScriptCommand(const StaticString& name, Allocator& allocator)
			: name(name, &allocator)
			, args(allocator)
		{
		}
	};

	class ScriptCodePrivate
	{
	public:
		ScriptCodePrivate(String&& text, Allocator& allocator)
			: _allocator(allocator)
		{
			ScriptScanner scanner(text);
			for (ScriptCommand* command = nullptr;;)
			{
				const auto token = scanner.read();
				if (token.type == ScriptScanner::Token::End)
					break;
				if (!command)
				{
					switch (token.type)
					{
					case ScriptScanner::Token::Identifier:
					case ScriptScanner::Token::XIdentifier:
						_commands.emplace_back(token.string, allocator);
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
						command->args.emplace_back(new(_temporaries.allocate())
							ScriptValue(token.string, ScriptValue::Type::Name, allocator));
						break;

					case ScriptScanner::Token::XIdentifier:
					case ScriptScanner::Token::Literal:
						command->args.emplace_back(new(_temporaries.allocate())
							ScriptValue(token.string, ScriptValue::Type::Literal, allocator));
						break;

					case ScriptScanner::Token::String:
						command->args.emplace_back(new(_temporaries.allocate())
							ScriptValue(token.string, ScriptValue::Type::String, allocator));
						break;

					case ScriptScanner::Token::Separator:
						command = nullptr;
						break;

					default:
						throw DataError("[", token.line, ":", token.column, "] Unexpected token");
					}
				}
			}
		}

	public:
		Allocator& _allocator;
		StdVector<ScriptCommand> _commands{ _allocator };
		Pool<ScriptValue> _temporaries{ 32, _allocator };
	};

	ScriptCode::ScriptCode() = default;

	ScriptCode::ScriptCode(String&& text, Allocator& allocator)
		: _private(std::make_unique<ScriptCodePrivate>(std::move(text), allocator))
	{
	}

	ScriptCode::ScriptCode(const StaticString& text, Allocator& allocator)
		: ScriptCode(String(text, &allocator), allocator) // The script text must be mutable for in-place parsing.
	{
	}

	void ScriptCode::execute(ScriptContext& context) const
	{
		if (!_private)
			return;
		TemporaryAllocator<64> result_allocator(_private->_allocator);
		String result(&result_allocator);
		for (const auto& command : _private->_commands)
			if (!context.call(command.name, &result, ScriptArgs(context, command.args)))
				break;
	}

	ScriptCode ScriptCode::load(const StaticString& filename, Allocator& allocator)
	{
		String text(&allocator);
		return Reader(filename).read_all(text) ? ScriptCode(std::move(text), allocator) : ScriptCode();
	}

	ScriptCode::~ScriptCode() = default;
	ScriptCode::ScriptCode(ScriptCode&&) noexcept = default;
	ScriptCode& ScriptCode::operator=(ScriptCode&&) noexcept = default;
}
