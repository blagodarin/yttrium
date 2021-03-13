// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <type_traits>

namespace Yt
{
	class Bgra32
	{
	public:
		uint8_t _b, _g, _r, _a;

		Bgra32() noexcept = default;

		template <typename B, typename G, typename R, typename = std::enable_if_t<std::is_integral_v<B> && std::is_integral_v<G> && std::is_integral_v<R>>>
		constexpr explicit Bgra32(B b, G g, R r) noexcept
			: _b{ static_cast<uint8_t>(b) }, _g{ static_cast<uint8_t>(g) }, _r{ static_cast<uint8_t>(r) }, _a{ 0xff } {}

		template <typename B, typename G, typename R, typename A, typename = std::enable_if_t<std::is_integral_v<B> && std::is_integral_v<G> && std::is_integral_v<R> && std::is_integral_v<A>>>
		constexpr explicit Bgra32(B b, G g, R r, A a) noexcept
			: _b{ static_cast<uint8_t>(b) }, _g{ static_cast<uint8_t>(g) }, _r{ static_cast<uint8_t>(r) }, _a{ static_cast<uint8_t>(a) } {}

		static constexpr Bgra32 black(uint8_t alpha = 255) noexcept { return Bgra32{ 0, 0, 0, alpha }; }
		static constexpr Bgra32 blue(uint8_t alpha = 255) noexcept { return Bgra32{ 255, 0, 0, alpha }; }
		static constexpr Bgra32 cyan(uint8_t alpha = 255) noexcept { return Bgra32{ 255, 255, 0, alpha }; }
		static constexpr Bgra32 grayscale(uint8_t color, uint8_t alpha = 255) noexcept { return Bgra32{ color, color, color, alpha }; }
		static constexpr Bgra32 green(uint8_t alpha = 255) noexcept { return Bgra32{ 0, 255, 0, alpha }; }
		static constexpr Bgra32 magenta(uint8_t alpha = 255) noexcept { return Bgra32{ 255, 0, 255, alpha }; }
		static constexpr Bgra32 red(uint8_t alpha = 255) noexcept { return Bgra32{ 0, 0, 255, alpha }; }
		static constexpr Bgra32 white(uint8_t alpha = 255) noexcept { return Bgra32{ 255, 255, 255, alpha }; }
		static constexpr Bgra32 yellow(uint8_t alpha = 255) noexcept { return Bgra32{ 0, 255, 255, alpha }; }
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
}
