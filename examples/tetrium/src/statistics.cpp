#include "statistics.h"

#include <yttrium/script/context.h>

#include <string>

TetriumStatistics::TetriumStatistics(ScriptContext& script)
	: _script(script)
	, _statistics
		{
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