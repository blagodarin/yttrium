#ifndef _src_ion_utils_h_
#define _src_ion_utils_h_

#include <yttrium/ion/utils.h>

#include <string>

namespace Yttrium
{
	class IonObject;

	namespace Ion
	{
		std::string serialize(const IonObject&, bool root, int indentation);
	}
}

#endif
