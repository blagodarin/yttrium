#include <yttrium/margins.h>

#include <yttrium/size.h>

namespace Yttrium
{
	Size Margins::min_size() const
	{
		return Size(left + 1 + right, top + 1 + bottom);
	}
}
