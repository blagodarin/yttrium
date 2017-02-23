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

	/// Script call description.
	class ScriptCall
	{
		friend ScriptContext;

	public:

		ScriptContext&     context;  ///< Calling context.
		const std::string& function; ///< Function name, guaranteed to be non-empty.
		std::string&       result;   ///< Function result.
		const ScriptArgs&  args;     ///< Function arguments.

	private:

		ScriptCall(ScriptContext& context, const std::string& function, std::string& result, const ScriptArgs& args)
			: context(context), function(function), result(result), args(args) {}
	};

	/// Script context.
	class Y_API ScriptContext
	{
	public:
		///
		using Command = std::function<void(const ScriptCall&)>;

		///
		ScriptContext(Allocator* allocator = DefaultAllocator);

		///
		ScriptContext(ScriptContext* parent, Allocator* allocator = nullptr);

		///
		~ScriptContext();

		///
		Allocator& allocator() const noexcept;

		/// Call a command.
		bool call(const std::string& name, std::string& result, const ScriptArgs&);

		/// Define a command.
		void define(const std::string& name, size_t min_args, size_t max_args, const Command&);

		///
		void define(const std::string& name, size_t args, const Command& command)
		{
			define(name, args, args, command);
		}

		///
		void define(const std::string& name, const Command& command)
		{
			define(name, 0, 0, command);
		}

		/// Find a value by name.
		/// \param name Value name.
		/// \return Value pointer or \c nullptr if no such value exist.
		ScriptValue* find(const std::string& name) const;

		///
		int get_int(const std::string& name, int default_value);

		/// Get the root context.
		/// \return Root context.
		ScriptContext& root();

		///
		void set(const std::string& name, int value);

		///
		void set(const std::string& name, const std::string& value);

		/// Substitutes script variables in a string.
		/// Every occurence of curly brace pair is threated as a variable reference.
		void substitute(std::string& target, const std::string& source) const;

	private:
		const std::unique_ptr<class ScriptContextPrivate> _private;
	};
}

#endif
