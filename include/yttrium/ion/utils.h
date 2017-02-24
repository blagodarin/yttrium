/// \file
/// \brief

#ifndef _include_yttrium_ion_utils_h_
#define _include_yttrium_ion_utils_h_

#include <yttrium/api.h>

namespace Yttrium
{
	class IonNode;
	class IonObject;
	class StaticString;

	/// ION facilities.
	namespace Ion
	{
		///
		Y_API void append(IonObject& target, const IonNode& source);

		///
		Y_API bool get(const IonNode& source, const StaticString*& value);
	}
}

#endif
