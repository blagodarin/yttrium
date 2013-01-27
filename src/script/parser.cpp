#include "parser.h"

#include <Yttrium/script/context.h> // NOTE: Just for execute().
#include <Yttrium/script/value.h>

#include "logging.h"
#include "scanner.h"

namespace Yttrium
{

namespace
{

enum class ParserState
{
	Initial,
	Command,
};

} // namespace

ScriptParser::ScriptParser(ScriptContext *context, Allocator *allocator)
	: _context(context)
	, _allocator(allocator)
	, _temporaries(32, _allocator) // NOTE: Magic number!
{
}

bool ScriptParser::parse(const StaticString &script)
{
	Y_LOG_TRACE(S("[ScriptParser] Parsing \"") << script << S("\"..."));

	String mutable_script(script, _allocator);

	ScriptScanner scanner(mutable_script);

	ScriptScanner::Token token;

	Command *command = nullptr;

	_commands.clear();
	_temporaries.clear();

	ParserState state = ParserState::Initial;

	while (scanner.read(&token) && token.type != ScriptScanner::Token::End)
	{
		switch (state)
		{
		case ParserState::Initial:

			switch (token.type)
			{
			case ScriptScanner::Token::Identifier:
			case ScriptScanner::Token::XIdentifier:

				Y_LOG_TRACE(S("[ScriptParser]  - identifier \"") << token.string << '\"');
				_commands.push_back(Command(token.string, _context, _allocator));
				command = &_commands.back();
				state = ParserState::Command;
				break;

			case ScriptScanner::Token::Separator:

				Y_LOG_TRACE(S("[ScriptParser]  - ;"));
				break;

			default:

				Y_LOG_TRACE(S("[ScriptParser]  - unexpected token type ") << token.type
					<< S(": \"") << token.string << '\"');
				_commands.clear();
				return false;
			}
			break;

		case ParserState::Command:

			switch (token.type)
			{
			case ScriptScanner::Token::Identifier:

				Y_LOG_TRACE(S("[ScriptParser]  - identifier \"") << token.string << '\"');
				command->args._values.push_back(
					new(_temporaries.allocate()) ScriptValue(token.string, ScriptValue::Name, _allocator));
				break;

			case ScriptScanner::Token::XIdentifier:
			case ScriptScanner::Token::Literal:

				Y_LOG_TRACE(S("[ScriptParser]  - literal \"") << token.string << '\"');
				command->args._values.push_back(
					new(_temporaries.allocate()) ScriptValue(token.string, ScriptValue::Literal, _allocator));
				break;

			case ScriptScanner::Token::String:

				Y_LOG_TRACE(S("[ScriptParser]  - string \"") << token.string << '\"');
				command->args._values.push_back(
					new(_temporaries.allocate()) ScriptValue(token.string, ScriptValue::String, _allocator));
				break;

			case ScriptScanner::Token::Separator:

				state = ParserState::Initial;
				break;

			default:

				Y_LOG_TRACE(S("[ScriptParser]  - unexpected token type ") << token.type
					<< S(": \"") << token.string << '\"');
				_commands.clear();
				return false;
			}
			break;
		}
	}

	if (token.type != ScriptScanner::Token::End)
	{
		Y_LOG(S("[ScriptParser] Scanner error"));
		_commands.clear();
		return false;
	}

	Y_LOG_TRACE(S("[ScriptParser] Commands parsed: ") << _commands.size());

	return true;
}

bool ScriptParser::execute(ExecutionMode mode)
{
	for (Commands::iterator i = _commands.begin(); i != _commands.end(); ++i)
	{
		String name = i->name;

		if (mode != ExecutionMode::Do)
		{
			if (name[0] != '+')
			{
				continue;
			}

			name[0] = '-';
		}

		if (!_context->call(name, &i->result, i->args))
		{
			return false;
		}
	}

	return true;
}

} // namespace Yttrium
