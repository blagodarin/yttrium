/// \file
/// \brief Script context.

#ifndef _include_yttrium_script_context_h_
#define _include_yttrium_script_context_h_

#include <yttrium/memory/global.h>

#include <functional>
#include <memory>

namespace Yttrium
{
	class ScriptArgs;
	class ScriptContext;
	class ScriptValue;
	class StaticString;
	class String;

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
	public:

		///
		using Command = std::function<void(const ScriptCall&)>;

	public:

		///
		ScriptContext(Allocator* allocator = DefaultAllocator);

		///
		ScriptContext(ScriptContext* parent, Allocator* allocator = nullptr);

		///
		Allocator& allocator() const noexcept;

		/// Call a command.
		/// \param name Command name.
		/// \param result Command result.
		/// \param args Command arguments.
		/// \return \c true if the command was succesfully called.
		bool call(const StaticString& name, String* result, const ScriptArgs& args);

		/// Define a command.
		/// \param name Command name.
		/// \param min_args Minimum number of arguments.
		/// \param max_args Maximum number of arguments.
		/// \param command Command handler.
		void define(const StaticString& name, size_t min_args, size_t max_args, const Command& command);

		///
		void define(const StaticString& name, size_t args, const Command& command)
		{
			define(name, args, args, command);
		}

		///
		void define(const StaticString& name, const Command& command)
		{
			define(name, 0, 0, command);
		}

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

		/// Substitutes script variables in a string.
		/// Every occurence of curly brace pair is threated as a variable reference.
		void substitute(std::string& target, const StaticString& source) const;

		/// Undefine a command.
		/// \param name Command name.
		void undefine(const StaticString& name);

		/// Unset a variable.
		/// \param name Variable name.
		void unset(const StaticString& name);

	private:
		const std::unique_ptr<class ScriptContextPrivate> _private;

	public:
		~ScriptContext();
	};
}

#endif
