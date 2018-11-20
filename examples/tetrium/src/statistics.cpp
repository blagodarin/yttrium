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

#include "statistics.h"

#include <yttrium/script/context.h>

#include <string>

TetriumStatistics::TetriumStatistics(ScriptContext& script)
	: _script(script)
	, _statistics{
		{ 100000, "John Placeholder" },
		{ 50000, "John Placeholder" },
		{ 10000, "John Placeholder" },
		{ 5000, "John Placeholder" },
		{ 1000, "John Placeholder" },
	}
{
	update_variables();
}

void TetriumStatistics::update(int score, const std::string& name)
{
	_statistics.emplace(score, name);
	_statistics.erase(_statistics.begin());
	update_variables();
}

void TetriumStatistics::update_variables() const
{
	int index = 0;
	for (auto i = _statistics.rbegin(); i != _statistics.rend(); ++i)
	{
		++index;
		_script.set("name" + std::to_string(index), i->second);
		_script.set("score" + std::to_string(index), i->first);
	}
}
