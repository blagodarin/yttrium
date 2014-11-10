#ifndef __SCRIPT_PARSER_H
#define __SCRIPT_PARSER_H

#include <yttrium/pool.h>
#include <yttrium/script/args.h>

namespace Yttrium
{

class ScriptParser
{
public:

	ScriptParser(ScriptContext& context, Allocator* allocator);

public:

	bool parse(const StaticString& script);

	bool execute(ExecutionMode mode);

private:

	class Command
	{
	public:

		String     name;
		String     result;
		ScriptArgs args;

		Command(const StaticString& name, ScriptContext& context, Allocator* allocator)
			: name(name, allocator)
			, result(allocator)
			, args(context)
		{
		}
	};

private:

	ScriptContext&       _context;
	Allocator*           _allocator;
	std::vector<Command> _commands;
	Pool<ScriptValue>    _temporaries;
};

} // namespace Yttrium

#endif // __SCRIPT_PARSER_H
