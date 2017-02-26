/// \file
/// \brief

#ifndef _include_yttrium_math_euler_h_
#define _include_yttrium_math_euler_h_

namespace Yttrium
{
	/// Euler angles.
	class Euler
	{
	public:
		float yaw = 0;   ///< Horizontal rotation angle.
		float pitch = 0; ///< Vertical rotation angle.
		float roll = 0;  ///<

		Euler() = default;
		Euler(float yaw_, float pitch_, float roll_) : yaw(yaw_), pitch(pitch_), roll(roll_) {}
	};
}

#endif
