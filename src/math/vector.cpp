#include <yttrium/vector.h>

#include <cmath>

namespace Yttrium
{
	float Vector4::length() const
	{
		return ::sqrt(x * x + y * y + z * z);
	}

	Vector4 Vector4::normalized() const
	{
		return *this / length();
	}
}
