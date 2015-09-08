/// \file
/// \brief

#ifndef __Y_ION_UTILS_H
#define __Y_ION_UTILS_H

#include <yttrium/global.h>

namespace Yttrium
{
	class IonObject;
	class IonValue;
	class StaticString;
	class Vector2;

	namespace Ion
	{
		///
		Y_API bool read(const IonValue& source, Vector2& value);

		///
		Y_API bool read(const IonObject& source, const StaticString& name, float& value);

		///
		Y_API bool read(const IonObject& source, const StaticString& name, const IonObject*& value);
	}
}

#endif
