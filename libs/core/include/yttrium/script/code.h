//
// Copyright 2019 Sergei Blagodarin
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

#pragma once

#include <yttrium/api.h>

#include <memory>
#include <string>

namespace Yttrium
{
	class ScriptContext;

	/// Parsed script code.
	class Y_CORE_API ScriptCode
	{
	public:
		ScriptCode();
		ScriptCode(const ScriptCode&);
		ScriptCode(ScriptCode&&) noexcept;
		~ScriptCode() noexcept;
		ScriptCode& operator=(const ScriptCode&) = delete;
		ScriptCode& operator=(ScriptCode&&) noexcept;

		///
		explicit ScriptCode(std::string&&);

		///
		explicit operator bool() const noexcept { return static_cast<bool>(_private); }

		/// Executes the script.
		void execute(ScriptContext&) const;

	private:
		std::unique_ptr<class ScriptCodePrivate> _private;
	};
}
