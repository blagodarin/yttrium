#include "api.h"

#include <cstring>

namespace Yttrium
{
	namespace Gl
	{
		bool check_extension(const char* list, const char* name)
		{
			const size_t name_size = ::strlen(name);
			while ((list = ::strstr(list, name)))
			{
				list += name_size;
				if (*list == ' ' || *list == 0)
					return true;
			}
			return false;
		}
	}
}
