#include <yttrium/script/code.h>

#include <yttrium/file.h>
#include <yttrium/memory/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>
#include "scanner.h"

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
		ScriptCodePrivate(Allocator& allocator)
			: _commands(allocator)
			, _temporaries(32, allocator)
			, _last_result(&allocator)
		{
		}

	public:
		StdVector<ScriptCommand> _commands;
		Pool<ScriptValue> _temporaries;
		String _last_result;
	};

	ScriptCode::ScriptCode() = default;

	ScriptCode::ScriptCode(String&& text, Allocator& allocator)
	{
		auto&& code = make_unique<ScriptCodePrivate>(allocator, allocator);

		ScriptScanner scanner(text);

		ScriptScanner::Token token;

		ScriptCommand* command = nullptr;

		enum class ParserState
		{
			Initial,
			Command,
		};

		ParserState state = ParserState::Initial;

		while (scanner.read(token) && token.type != ScriptScanner::Token::End)
		{
			switch (state)
			{
			case ParserState::Initial:
				switch (token.type)
				{
				case ScriptScanner::Token::Identifier:
				case ScriptScanner::Token::XIdentifier:
					code->_commands.emplace_back(token.string, allocator);
					command = &code->_commands.back();
					state = ParserState::Command;
					break;

				case ScriptScanner::Token::Separator:
					break;

				default:
					return;
				}
				break;

			case ParserState::Command:
				switch (token.type)
				{
				case ScriptScanner::Token::Identifier:
					command->args.emplace_back(new(code->_temporaries.allocate())
						ScriptValue(token.string, ScriptValue::Type::Name, allocator));
					break;

				case ScriptScanner::Token::XIdentifier:
				case ScriptScanner::Token::Literal:
					command->args.emplace_back(new(code->_temporaries.allocate())
						ScriptValue(token.string, ScriptValue::Type::Literal, allocator));
					break;

				case ScriptScanner::Token::String:
					command->args.emplace_back(new(code->_temporaries.allocate())
						ScriptValue(token.string, ScriptValue::Type::String, allocator));
					break;

				case ScriptScanner::Token::Separator:
					state = ParserState::Initial;
					break;

				default:
					return;
				}
				break;
			}
		}

		if (token.type != ScriptScanner::Token::End)
			return;

		_private = std::move(code);
	}

	ScriptCode::ScriptCode(const StaticString& text, Allocator& allocator)
		: ScriptCode(String(text, &allocator), allocator) // The script text must be mutable for in-place parsing.
	{
	}

	void ScriptCode::execute(ScriptContext& context, ScriptCodeMode mode) const
	{
		if (!_private)
			return;

		for (auto& command: _private->_commands)
		{
			bool revert_mode = false;

			if (mode == ScriptCodeMode::Undo)
			{
				if (command.name[0] != '+')
					continue;
				command.name[0] = '-'; // TODO: Replace this unsafe anti-allocation trick with a better one.
				revert_mode = true;
			}

			const bool result = context.call(command.name, &_private->_last_result, ScriptArgs(context, command.args));

			if (revert_mode)
				command.name[0] = '+';

			if (!result)
				break;
		}
	}

	ScriptCode ScriptCode::load(const StaticString& filename, Allocator& allocator)
	{
		String text(&allocator);
		return File(filename, &allocator).read_all(&text) ? ScriptCode(std::move(text), allocator) : ScriptCode();
	}

	ScriptCode::~ScriptCode() = default;
	ScriptCode::ScriptCode(ScriptCode&&) noexcept = default;
	ScriptCode& ScriptCode::operator=(ScriptCode&&) noexcept = default;
}
