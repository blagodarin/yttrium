#ifndef _include_yttrium_math_euler_h_
#define _include_yttrium_math_euler_h_

namespace Yttrium
{
	/// Euler angles.
	class Euler
	{
	public:
		float yaw;   ///< Horizontal rotation angle.
		float pitch; ///< Vertical rotation angle.
		float roll;  ///<

		Euler() noexcept = default;
		constexpr Euler(float y, float p, float r) noexcept : yaw{y}, pitch{p}, roll{r} {}
	};
}

#endif
