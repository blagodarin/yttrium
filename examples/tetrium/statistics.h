#ifndef _examples_tetrium_statistics_h_
#define _examples_tetrium_statistics_h_

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

#endif
