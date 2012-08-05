#include "api.h"

#include <cstring> // strlen, strstr

namespace Yttrium
{

namespace Gl
{

bool check_extension(const char *list, const char *name)
{
	size_t length = strlen(name);

	while ((list = strstr(list, name)) != 0)
	{
		list += length;
		if (*list == ' ' || *list == 0)
		{
			return true;
		}
	}

	return false;
}

} // namespace Gl

} // namespace Yttrium
