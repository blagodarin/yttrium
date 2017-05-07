#ifndef _include_yttrium_ion_utils_h_
#define _include_yttrium_ion_utils_h_

#include <yttrium/api.h>
#include <yttrium/std/string_view.h>

namespace Yttrium
{
	class IonNode;
	class IonObject;

	/// ION facilities.
	namespace Ion
	{
		///
		Y_API void append(IonObject& target, const IonNode& source);

		///
		Y_API bool get(const IonNode& source, std::string_view& value);
	}
}

#endif
