//
// Copyright 2018 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _include_yttrium_script_context_h_
#define _include_yttrium_script_context_h_

#include <yttrium/api.h>

#include <functional>
#include <memory>
#include <string_view>

namespace Yttrium
{
	class ScriptArgs;
	class ScriptContext;
	class ScriptValue;

	/// Script call description.
	class ScriptCall
	{
	public:
		ScriptContext& _context;      ///< Calling context.
		const std::string& _function; ///< Function name, guaranteed to be non-empty.
		std::string& _result;         ///< Function result.
		const ScriptArgs& _args;      ///< Function arguments.

	private:
		ScriptCall(ScriptContext& context, const std::string& function, std::string& result, const ScriptArgs& args)
			: _context{ context }, _function{ function }, _result{ result }, _args{ args } {}
		friend ScriptContext;
	};

	/// Script context.
	class Y_CORE_API ScriptContext
	{
	public:
		///
		using Command = std::function<void(const ScriptCall&)>;

		///
		explicit ScriptContext(ScriptContext* parent = nullptr);

		///
		~ScriptContext();

		/// Calls a command.
		bool call(const std::string& name, std::string& result, const ScriptArgs&);
		bool call(const std::string& name, const std::vector<std::string>&);

		/// Defines a command.
		void define(const std::string& name, size_t min_args, size_t max_args, const Command&);
		void define(const std::string& name, size_t num_args, const Command& command) { define(name, num_args, num_args, command); }
		void define(const std::string& name, const Command& command) { define(name, 0, 0, command); }

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
		void set(const std::string& name, int);

		///
		void set(const std::string& name, std::string_view);

		/// Substitutes script variables in a string.
		/// Every occurence of curly brace pair is threated as a variable reference.
		void substitute(std::string& target, std::string_view source) const;

	private:
		const std::unique_ptr<class ScriptContextPrivate> _private;
	};
}

#endif
