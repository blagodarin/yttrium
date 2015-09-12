/// \file
/// \brief Script context.

#ifndef __Y_SCRIPT_CONTEXT_H
#define __Y_SCRIPT_CONTEXT_H

#include <yttrium/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/code.h>
#include <yttrium/script/value.h>
#include <yttrium/string.h>

#include <functional>
#include <map>

namespace Yttrium
{
	class ScriptContext;

	/// Script call description.
	class ScriptCall
	{
		friend ScriptContext;

	public:

		ScriptContext&      context;  ///< Calling context.
		const StaticString& function; ///< Function name, guaranteed to be non-empty.
		const ScriptArgs&   args;     ///< Function arguments.
		String&             result;   ///< Function result.

	private:

		ScriptCall(ScriptContext& context, const StaticString& function, const ScriptArgs& args, String& result)
			: context(context)
			, function(function)
			, args(args)
			, result(result)
		{
		}
	};

	/// Script context.
	class Y_API ScriptContext
	{
		friend ScriptArgs;

		Y_NONCOPYABLE(ScriptContext);

	public:

		///
		typedef std::function<void(const ScriptCall&)> Command;

	public:

		///
		ScriptContext(Allocator* allocator = DefaultAllocator);

		///
		ScriptContext(ScriptContext* parent, Allocator* allocator = nullptr);

	public:

		/// Call a command.
		/// \param name Command name.
		/// \param result Command result.
		/// \param args Command arguments.
		/// \return \c true if the command was succesfully called.
		bool call(const StaticString& name, String* result, const ScriptArgs &args);

		/// Define a command.
		/// \param name Command name.
		/// \param min_args Minimum number of arguments.
		/// \param max_args Maximum number of arguments.
		/// \param command Command handler.
		void define(const StaticString& name, size_t min_args, size_t max_args, const Command& command);

		///
		void define(const StaticString &name, size_t args, const Command& command)
		{
			define(name, args, args, command);
		}

		///
		void define(const StaticString &name, const Command& command)
		{
			define(name, 0, 0, command);
		}

		/// Execute the specified script.
		/// \param script Script text to execute.
		/// \param mode Execution mode.
		/// \return \c true on success.
		bool execute(const StaticString& script, ScriptCode::ExecutionMode mode = ScriptCode::Do);

		/// Execute the script from the specified file.
		/// \param name Script file to execute.
		/// \return \c true on success.
		bool execute_file(const StaticString& name);

		/// Find a value by name.
		/// \param name Value name.
		/// \return Value pointer or \c nullptr if no such value exist.
		ScriptValue* find(const StaticString& name) const;

		///
		int get_int(const StaticString& name, int default_value);

		/// Get the root context.
		/// \return Root context.
		ScriptContext& root();

		///
		const ScriptValue* set(const StaticString& name, int value);

		/**
		* \overload
		* \param name
		* \param value
		* \return
		*/
		const ScriptValue* set(const StaticString& name, double value);

		/**
		* \overload
		* \param name
		* \param value
		* \return
		*/
		const ScriptValue* set(const StaticString& name, const StaticString& value);

		/// Substitute script variables in a string.
		/// \param target
		/// \param source
		/// \note Every occurence of the pair of curly braces is threated as a variable name.
		void substitute(String& target, const StaticString& source) const;

		/// Undefine a command.
		/// \param name Command name.
		void undefine(const StaticString& name);

		/// Unset a variable.
		/// \param name Variable name.
		void unset(const StaticString& name);

	public:

		/// Get the root context of global script manager.
		static ScriptContext& global();

	private:

		struct CommandContext
		{
			Command command;
			size_t  min_args;
			size_t  max_args;

			CommandContext() {}
			
			CommandContext(Command command, size_t min_args, size_t max_args)
				: command(command)
				, min_args(min_args)
				, max_args(max_args)
			{
			}
		};

		Allocator*                       _allocator;
		ScriptContext*                   _parent;
		Pool<ScriptValue>                _value_pool;
		std::map<String, ScriptValue*>   _values;
		std::map<String, CommandContext> _commands;
	};
}

#endif
