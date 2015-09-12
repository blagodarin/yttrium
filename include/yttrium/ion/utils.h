/// \file
/// \brief

#ifndef __Y_ION_UTILS_H
#define __Y_ION_UTILS_H

#include <yttrium/global.h>

namespace Yttrium
{
	class IonNode;
	class IonObject;
	class IonValue;
	class StaticString;
	class Vector2;

	/// ION facilities.
	namespace Ion
	{
		///
		Y_API void append(IonObject& target, const IonNode& source);

		///
		Y_API bool get(const IonValue& source, Vector2& value);

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
