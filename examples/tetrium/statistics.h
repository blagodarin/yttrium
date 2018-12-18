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

#pragma once

#include <map>

namespace Yttrium
{
	class ScriptContext;
}

using namespace Yttrium;

class TetriumStatistics
{
public:
	explicit TetriumStatistics(ScriptContext&);

	void update(int score, const std::string& name);

private:
	void update_variables() const;

private:
	ScriptContext& _script;
	std::multimap<int, std::string> _statistics;
};
