#ifndef _src_script_context_h_
#define _src_script_context_h_

#include <yttrium/script/context.h>

#include <yttrium/memory/pool.h>
#include <yttrium/std/map.h>
#include "../base/private_base.h"

namespace Yttrium
{
	struct Y_PRIVATE ScriptContext::Private : public PrivateBase<ScriptContext::Private>
	{
		struct CommandContext
		{
			Command command;
			size_t min_args = 0;
			size_t max_args = 0;

			CommandContext() = default;

			CommandContext(Command command, size_t min_args, size_t max_args)
				: command(command)
				, min_args(min_args)
				, max_args(max_args)
			{
			}
		};

		ScriptContext* const           _parent;
		Pool<ScriptValue>              _value_pool;
		StdMap<String, ScriptValue*>   _values;
		StdMap<String, CommandContext> _commands;

		Private(ScriptContext* parent, Allocator& allocator)
			: PrivateBase(&allocator)
			, _parent(parent)
			, _value_pool(32, _allocator)
			, _values(allocator)
			, _commands(allocator)
		{
		}
	};
}

#endif
