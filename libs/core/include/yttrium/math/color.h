//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

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
		constexpr explicit Bgra32(B b, G g, R r) noexcept
			: _b{ static_cast<std::uint8_t>(b) }
			, _g{ static_cast<std::uint8_t>(g) }
			, _r{ static_cast<std::uint8_t>(r) }
			, _a{ 0xff }
		{
		}

		template <typename B, typename G, typename R, typename A, typename = std::enable_if_t<std::is_integral_v<B> && std::is_integral_v<G> && std::is_integral_v<R> && std::is_integral_v<A>>>
		constexpr explicit Bgra32(B b, G g, R r, A a) noexcept
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
