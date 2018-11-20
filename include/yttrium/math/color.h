#ifndef _include_yttrium_math_color_h_
#define _include_yttrium_math_color_h_

#include <cstdint>
#include <type_traits>

namespace Yttrium
{
	class Bgra32
	{
	public:
		std::uint8_t _b, _g, _r, _a;

		Bgra32() noexcept = default;

		template <typename B, typename G, typename R, typename = std::enable_if_t<std::is_integral_v<B> && std::is_integral_v<G> && std::is_integral_v<R>>>
		explicit constexpr Bgra32(B b, G g, R r) noexcept
			: _b{ static_cast<std::uint8_t>(b) }
			, _g{ static_cast<std::uint8_t>(g) }
			, _r{ static_cast<std::uint8_t>(r) }
			, _a{ 0xff }
		{
		}

		template <typename B, typename G, typename R, typename A, typename = std::enable_if_t<std::is_integral_v<B> && std::is_integral_v<G> && std::is_integral_v<R> && std::is_integral_v<A>>>
		explicit constexpr Bgra32(B b, G g, R r, A a) noexcept
			: _b{ static_cast<std::uint8_t>(b) }
			, _g{ static_cast<std::uint8_t>(g) }
			, _r{ static_cast<std::uint8_t>(r) }
			, _a{ static_cast<std::uint8_t>(a) }
		{
		}
	};

	class Color4f
	{
	public:
		float _r, _g, _b, _a;

		Color4f() noexcept = default;
		constexpr Color4f(float r, float g, float b, float a = 1.f) noexcept
			: _r{ r }, _g{ g }, _b{ b }, _a{ a } {}
	};

	constexpr bool operator==(const Bgra32& a, const Bgra32& b) noexcept { return a._b == b._b && a._g == b._g && a._r == b._r && a._a == b._a; }
	constexpr bool operator!=(const Bgra32& a, const Bgra32& b) noexcept { return !(a == b); }
}

#endif
