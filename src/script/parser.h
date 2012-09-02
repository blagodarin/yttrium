#ifndef __SCRIPT_PARSER_H
#define __SCRIPT_PARSER_H

#include <Yttrium/script/context.h>

namespace Yttrium
{

class ScriptParser
{
public:

	ScriptParser(ScriptContext *context, Allocator *allocator);

public:

	bool parse(const StaticString &script);

	bool execute(ScriptContext::ExecutionMode mode);

private:

	class Command
	{
	public:

		String     name;
		String     result;
		ScriptArgs args;

		Command(const StaticString &name, ScriptContext *context, Allocator *allocator)
			: name(name, allocator)
			, result(allocator)
			, args(context)
		{
		}
	};

	typedef std::vector<Command> Commands;

	typedef Pool<ScriptValue> Temporaries;

private:

	ScriptContext *_context;
	Allocator     *_allocator;
	Commands       _commands;
	Temporaries    _temporaries;
};

} // namespace Yttrium

#endif // __SCRIPT_PARSER_H
