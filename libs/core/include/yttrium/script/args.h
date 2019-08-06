//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <vector>

namespace Yt
{
	class ScriptCode;
	class ScriptContext;
	class ScriptValue;

	/// Script command arguments.
	class Y_CORE_API ScriptArgs
	{
	public:
		std::size_t size() const noexcept { return _values.size(); }

		const ScriptValue* operator[](std::size_t) const;

	private:
		ScriptContext& _context;
		const std::vector<ScriptValue*>& _values;
		ScriptArgs(ScriptContext& context, const std::vector<ScriptValue*>& values)
			: _context{ context }, _values{ values } {}
		friend ScriptCode;
		friend ScriptContext;
	};
}
