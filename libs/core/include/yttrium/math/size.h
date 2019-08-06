//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#include <yttrium/math/vector.h>

#include <cstddef>

namespace Yt
{
	class Size
	{
	public:
		int _width = 0;
		int _height = 0;

		constexpr Size() noexcept = default;
		constexpr Size(int w, int h) noexcept
			: _width{ w }, _height{ h } {}
		constexpr Size(std::size_t w, std::size_t h) noexcept
			: _width{ static_cast<int>(w) }, _height{ static_cast<int>(h) } {}
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
