/// \file
/// \brief

#ifndef _include_yttrium_ion_utils_h_
#define _include_yttrium_ion_utils_h_

#include <yttrium/global.h>

namespace Yttrium
{
	class IonNode;
	class IonObject;
	class IonValue;
	class StaticString;
	class PointF;

	/// ION facilities.
	namespace Ion
	{
		///
		Y_API void append(IonObject& target, const IonNode& source);

		///
		Y_API bool get(const IonValue& source, PointF& value);

		///
		Y_API bool get(const IonNode& source, const StaticString*& value);

		///
		Y_API bool get(const IonObject& source, const StaticString& name, float& value);

		///
		Y_API bool get(const IonObject& source, const StaticString& name, const StaticString*& value);

		///
		Y_API bool get(const IonObject& source, const StaticString& name, const IonObject*& value);

		///
		Y_API StaticString to_string(const IonObject& source, const StaticString& name);
	}
}

#endif
