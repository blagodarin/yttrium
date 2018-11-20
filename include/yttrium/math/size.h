#ifndef _include_yttrium_math_size_h_
#define _include_yttrium_math_size_h_

#include <yttrium/math/vector.h>

namespace Yttrium
{
	class Size
	{
	public:
		int _width = 0;
		int _height = 0;

		constexpr Size() noexcept = default;
		constexpr Size(int w, int h) noexcept
			: _width{ w }, _height{ h } {}
	};

	class SizeF
	{
	public:
		float _width = 0;
		float _height = 0;

		constexpr SizeF() noexcept = default;
		constexpr SizeF(float w, float h) noexcept
			: _width{ w }, _height{ h } {}
		constexpr explicit SizeF(const Size& s) noexcept
			: SizeF{ static_cast<float>(s._width), static_cast<float>(s._height) } {}
	};

	constexpr bool operator==(const Size& a, const Size& b) noexcept { return a._width == b._width && a._height == b._height; }
	constexpr bool operator!=(const Size& a, const Size& b) noexcept { return !(a == b); }

	constexpr SizeF operator*(const SizeF& size, float s) noexcept { return { size._width * s, size._height * s }; }
	constexpr SizeF operator*(const SizeF& size, const Vector2& v) noexcept { return { size._width * v.x, size._height * v.y }; }

	constexpr SizeF operator*(float s, const SizeF& size) noexcept { return size * s; }
	constexpr SizeF operator*(const Vector2& v, const SizeF& size) noexcept { return size * v; }

	constexpr SizeF operator/(const SizeF& size, float s) noexcept { return { size._width / s, size._height / s }; }
	constexpr SizeF operator/(const SizeF& size, const Vector2& v) noexcept { return { size._width / v.x, size._height / v.y }; }
}

#endif
