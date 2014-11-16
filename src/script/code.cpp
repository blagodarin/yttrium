#include <yttrium/script/code.h>

#include <yttrium/file.h>
#include <yttrium/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/context.h>
#include <yttrium/script/value.h>

#include "../base/private_base.h"
#include "manager.h"
#include "scanner.h"

namespace Yttrium
{

class Y_PRIVATE ScriptCode::Private: public PrivateBase<ScriptCode::Private>
{
public:

	Private(Allocator* allocator)
		: PrivateBase(allocator)
		, _temporaries(32, allocator) // TODO: Get rid of magic numbers.
		, _last_result(allocator)
	{
	}

	~Private() = default;

public:

	struct Command
	{
		String name;
		std::vector<ScriptValue*> args;

		Command(const StaticString& name, Allocator* allocator): name(name, allocator) {}
	};

	std::vector<Command> _commands;
	Pool<ScriptValue>    _temporaries;
	String               _last_result;
};

Y_IMPLEMENT_PRIVATE(ScriptCode);

ScriptCode::ScriptCode(String&& text)
	: ScriptCode()
{
	PrivateHolder<ScriptCode::Private> code(script_manager_allocator);

	ScriptScanner scanner(text);

	ScriptScanner::Token token;

	Private::Command* command = nullptr;

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
				code->_commands.emplace_back(token.string, script_manager_allocator);
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
					ScriptValue(token.string, ScriptValue::Name, script_manager_allocator));
				break;

			case ScriptScanner::Token::XIdentifier:
			case ScriptScanner::Token::Literal:
				command->args.emplace_back(new(code->_temporaries.allocate())
					ScriptValue(token.string, ScriptValue::Literal, script_manager_allocator));
				break;

			case ScriptScanner::Token::String:
				command->args.emplace_back(new(code->_temporaries.allocate())
					ScriptValue(token.string, ScriptValue::String, script_manager_allocator));
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

	_private = code.release();
}

ScriptCode::ScriptCode(const StaticString& text)
	: ScriptCode(String(text, script_manager_allocator)) // The script text must be mutable for in-place parsing.
{
}

void ScriptCode::execute(ScriptContext* context, ExecutionMode mode) const
{
	if (!_private)
		return;

	if (!context)
		context = &ScriptContext::global();

	for (auto& command: _private->_commands)
	{
		bool revert_mode = false;

		if (mode != ExecutionMode::Do)
		{
			if (command.name[0] != '+')
				continue;
			command.name[0] = '-'; // TODO: Replace this unsafe anti-allocation trick with a better one.
			revert_mode = true;
		}

		const bool result = context->call(command.name, &_private->_last_result, ScriptArgs(*context, command.args));

		if (revert_mode)
			command.name[0] = '+';

		if (!result)
			break;
	}
}

ScriptCode ScriptCode::load(const StaticString& filename)
{
	String text(script_manager_allocator);
	return File(filename, script_manager_allocator).read_all(&text) ? ScriptCode(text) : ScriptCode();
}

} // namespace Yttrium
