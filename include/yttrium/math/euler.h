#ifndef _include_yttrium_math_euler_h_
#define _include_yttrium_math_euler_h_

namespace Yttrium
{
	/// Euler angles.
	class Euler
	{
	public:
		float _yaw;   ///< Rotation around vertical axis.
		float _pitch; ///< Rotation around horizontal axis.
		float _roll;  ///<

		Euler() noexcept = default;
		constexpr Euler(float yaw, float pitch, float roll) noexcept : _yaw{yaw}, _pitch{pitch}, _roll{roll} {}
	};
}

#endif
