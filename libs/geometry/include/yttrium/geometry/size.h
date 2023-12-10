// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <seir_math/vec.hpp>

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
		constexpr Size(size_t w, size_t h) noexcept
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
	constexpr bool operator==(const SizeF& a, const SizeF& b) noexcept { return a._width == b._width && a._height == b._height; }

	constexpr SizeF operator*(const SizeF& size, float s) noexcept { return { size._width * s, size._height * s }; }
	constexpr SizeF operator*(const SizeF& size, const seir::Vec2& v) noexcept { return { size._width * v.x, size._height * v.y }; }

	constexpr SizeF operator*(float s, const SizeF& size) noexcept { return size * s; }
	constexpr SizeF operator*(const seir::Vec2& v, const SizeF& size) noexcept { return size * v; }

	constexpr SizeF operator/(const SizeF& size, float s) noexcept { return { size._width / s, size._height / s }; }
	constexpr SizeF operator/(const SizeF& size, const seir::Vec2& v) noexcept { return { size._width / v.x, size._height / v.y }; }
}
