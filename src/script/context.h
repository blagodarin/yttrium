#ifndef _src_script_context_h_
#define _src_script_context_h_

#include <yttrium/script/context.h>

#include <yttrium/memory/pool.h>
#include <yttrium/std/map.h>

namespace Yttrium
{
	struct ScriptCommandContext
	{
		ScriptContext::Command command;
		size_t min_args = 0;
		size_t max_args = 0;

		ScriptCommandContext() = default;

		ScriptCommandContext(ScriptContext::Command command, size_t min_args, size_t max_args)
			: command(command)
			, min_args(min_args)
			, max_args(max_args)
		{
		}
	};

	class ScriptContext::Private
	{
	public:
		Private(Allocator& allocator, ScriptContext* parent = nullptr)
			: _parent(parent)
			, _value_pool(32, &allocator)
			, _values(allocator)
			, _commands(allocator)
		{
		}

		~Private() = default; // Prevents external visibility.

	public:
		ScriptContext* const _parent;
		Pool<ScriptValue> _value_pool;
		StdMap<String, ScriptValue*> _values;
		StdMap<String, ScriptCommandContext> _commands;
	};
}

#endif
