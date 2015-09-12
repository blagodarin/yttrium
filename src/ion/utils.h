#ifndef __ION_UTILS_H
#define __ION_UTILS_H

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
