/// \file
/// \brief Script context.

#ifndef __Y_SCRIPT_CONTEXT_H
#define __Y_SCRIPT_CONTEXT_H

#include <yttrium/pool.h>
#include <yttrium/script/args.h>
#include <yttrium/script/value.h>
#include <yttrium/string.h>

#include <functional>
#include <map>

namespace Yttrium
{

/// Script context.

class Y_API ScriptContext
{
	friend ScriptArgs;

	Y_NONCOPYABLE(ScriptContext);

public:

	///

	typedef std::map<String, String> Archive; // TODO: Think of keeping StaticStrings here.

	///

	typedef std::function<void (const StaticString &name, String *result, const ScriptArgs &args) noexcept> Command;

public:

	///

	ScriptContext(Allocator *allocator = DefaultAllocator) noexcept;

	///

	ScriptContext(ScriptContext *parent, Allocator *allocator = nullptr) noexcept;

public:

	///

	Archive archive() const noexcept;

	/// Call a command.
	/// \param name Command name.
	/// \param result Command result.
	/// \param args Command arguments.
	/// \return \c true if the command was succesfully called.

	bool call(const StaticString &name, String *result, const ScriptArgs &args) noexcept;

	/// Define a command.
	/// \param name Command name.
	/// \param command Command handler.
	/// \param min_args Minimum number of arguments.
	/// \param max_args Maximum number of arguments.

	void define(const StaticString &name, const Command &command, size_t min_args, size_t max_args) noexcept;

	/**
	* \overload
	* \param name Command name.
	* \param command Command handler.
	* \param args Number of arguments.
	*/

	inline void define(const StaticString &name, const Command &command, size_t args = 0) noexcept;

	/// Execute the specified script.
	/// \param script Script text to execute.
	/// \param mode Execution mode.
	/// \return \c true on success.

	bool execute(const StaticString &script, ExecutionMode mode = ExecutionMode::Do) noexcept;

	/// Execute the script from the specified file.
	/// \param name Script file to execute.
	/// \return \c true on success.

	bool execute_file(const StaticString &name) noexcept;

	/// Find a value by name.
	/// \param name Value name.
	/// \return Value pointer or \c nullptr if no such value exist.

	ScriptValue *find(const StaticString &name) const noexcept;

	/// Get the root context.
	/// \return Root context.

	ScriptContext *root() noexcept;

	///

	const ScriptValue *set(const StaticString &name, Integer value, ScriptValue::Flags flags = 0) noexcept;

	/**
	* \overload
	* \param name
	* \param value
	* \param flags
	* \return
	*/

	const ScriptValue *set(const StaticString &name, Real value, ScriptValue::Flags flags = 0) noexcept;

	/**
	* \overload
	* \param name
	* \param value
	* \param flags
	* \return
	*/

	const ScriptValue *set(const StaticString &name, const StaticString &value, ScriptValue::Flags flags = 0) noexcept;

	///

	void substitute(String *target, const StaticString &source) const noexcept;

	/// Substitute the script variables in a string.
	/// \param string Source string.
	/// \param allocator Allocator to use for the result.
	/// \return String with substitutions.
	/// \note Every occurence of the pair of curly braces is threated as a variable name.

	inline String substitute(const StaticString &string, Allocator *allocator = nullptr) const noexcept;

	/// Undefine a command.
	/// \param name Command name.

	void undefine(const StaticString &name) noexcept;

	/// Unset a variable.
	/// \param name Variable name.

	void unset(const StaticString &name) noexcept;

public:

	/// Get the root context of global script manager.
	static ScriptContext &global();

private:

	typedef Pool<ScriptValue> Values;

	struct Entity
	{
		ScriptValue *value;
		ScriptValue *archived_value;

		Entity(ScriptValue *value = nullptr, ScriptValue *archived_value = nullptr)
			: value(value)
			, archived_value(archived_value)
		{
		}
	};

	typedef std::map<String, Entity> Entities;

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

	typedef std::map<String, CommandContext> Commands;

private:

	Allocator     *_allocator;
	ScriptContext *_parent;
	Values         _values;
	Entities       _entities;
	Commands       _commands;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ScriptContext::define(const StaticString &name, const Command &command, size_t args) noexcept
{
	define(name, command, args, args);
}

String ScriptContext::substitute(const StaticString &string, Allocator *allocator) const noexcept
{
	String result(allocator ? allocator : _allocator);

	substitute(&result, string);
	return result;
}

} // namespace Yttrium

#endif // __Y_SCRIPT_CONTEXT_H