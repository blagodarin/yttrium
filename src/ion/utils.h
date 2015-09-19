#ifndef _src_ion_utils_h_
#define _src_ion_utils_h_

#include <yttrium/ion/utils.h>

namespace Yttrium
{
	class Allocator;
	class String;

	namespace Ion
	{
		String serialize(const IonObject& object, bool root, int indentation, Allocator* allocator);
	}
}

#endif
