#ifndef _include_yttrium_math_color_h_
#define _include_yttrium_math_color_h_

namespace Yttrium
{
	class Color4f
	{
	public:
		float r;
		float g;
		float b;
		float a;

		Color4f() noexcept = default;
		constexpr Color4f(float cr, float cg, float cb, float ca = 1) noexcept : r{cr}, g{cg}, b{cb}, a{ca} {}
	};
}

#endif
