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

#include <yttrium/math/matrix.h>
#include "iostream.h"

#include <catch2/catch.hpp>

namespace Yt
{
	constexpr bool operator==(const Matrix4& a, const Matrix4& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z && a.t == b.t; }
}

using Yt::Matrix4;

TEST_CASE("matrix4.construction")
{
	const Matrix4 m{
		// clang-format off
		11, 12, 13, 14,
		21, 22, 23, 24,
		31, 32, 33, 34,
		41, 42, 43, 44,
		// clang-format on
	};

	CHECK(m.x.x == 11.f);
	CHECK(m.y.x == 12.f);
	CHECK(m.z.x == 13.f);
	CHECK(m.t.x == 14.f);
	CHECK(m.x.y == 21.f);
	CHECK(m.y.y == 22.f);
	CHECK(m.z.y == 23.f);
	CHECK(m.t.y == 24.f);
	CHECK(m.x.z == 31.f);
	CHECK(m.y.z == 32.f);
	CHECK(m.z.z == 33.f);
	CHECK(m.t.z == 34.f);
	CHECK(m.x.w == 41.f);
	CHECK(m.y.w == 42.f);
	CHECK(m.z.w == 43.f);
	CHECK(m.t.w == 44.f);

	CHECK((&m.x.x)[0] == 11.f);
	CHECK((&m.x.x)[1] == 21.f);
	CHECK((&m.x.x)[2] == 31.f);
	CHECK((&m.x.x)[3] == 41.f);
	CHECK((&m.x.x)[4] == 12.f);
	CHECK((&m.x.x)[5] == 22.f);
	CHECK((&m.x.x)[6] == 32.f);
	CHECK((&m.x.x)[7] == 42.f);
	CHECK((&m.x.x)[8] == 13.f);
	CHECK((&m.x.x)[9] == 23.f);
	CHECK((&m.x.x)[10] == 33.f);
	CHECK((&m.x.x)[11] == 43.f);
	CHECK((&m.x.x)[12] == 14.f);
	CHECK((&m.x.x)[13] == 24.f);
	CHECK((&m.x.x)[14] == 34.f);
	CHECK((&m.x.x)[15] == 44.f);
}

TEST_CASE("matrix4.camera")
{
	const auto actual = Matrix4::camera({ 1, 2, 3 }, { 30, 45, 60 });
	const auto expected = Matrix4::rotation(60, { 0, -1, 0 }) * Matrix4::rotation(45, { -1, 0, 0 }) * Matrix4::rotation(30, { 0, 0, 1 }) * Matrix4::translation({ -1, -2, -3 });
	CHECK(actual.x.x == Approx{ expected.x.x }.epsilon(3e-5));
	CHECK(actual.x.y == Approx{ expected.x.y }.epsilon(3e-5));
	CHECK(actual.x.z == Approx{ expected.x.z }.epsilon(3e-5));
	CHECK(actual.x.w == Approx{ expected.x.w }.epsilon(3e-5));
	CHECK(actual.y.x == Approx{ expected.y.x }.epsilon(3e-5));
	CHECK(actual.y.y == Approx{ expected.y.y }.epsilon(3e-5));
	CHECK(actual.y.z == Approx{ expected.y.z }.epsilon(3e-5));
	CHECK(actual.y.w == Approx{ expected.y.w }.epsilon(3e-5));
	CHECK(actual.z.x == Approx{ expected.z.x }.epsilon(3e-5));
	CHECK(actual.z.y == Approx{ expected.z.y }.epsilon(3e-5));
	CHECK(actual.z.z == Approx{ expected.z.z }.epsilon(3e-5));
	CHECK(actual.z.w == Approx{ expected.z.w }.epsilon(3e-5));
	CHECK(actual.t.x == Approx{ expected.t.x }.epsilon(3e-5));
	CHECK(actual.t.y == Approx{ expected.t.y }.epsilon(3e-5));
	CHECK(actual.t.z == Approx{ expected.t.z }.epsilon(3e-5));
	CHECK(actual.t.w == Approx{ expected.t.w }.epsilon(3e-5));
}

TEST_CASE("matrix4.det")
{
	// Determinant "can be viewed as the scaling factor of the transformation described by the matrix" (c) Wikipedia.
	CHECK(det(Matrix4::identity()) == 1.f);
	CHECK(det(Matrix4::scaling(2)) == 2.f * 2.f * 2.f);
	CHECK(det(Matrix4::translation({ 2, 3, 5 })) == 1.f);
	CHECK(det(Matrix4::rotation(37, { 2, 3, 5 })) == Approx{ 1.f }.epsilon(2e-5));
	CHECK(det(Matrix4::scaling(4) * Matrix4::rotation(37, { 2, 3, 5 }) * Matrix4::scaling(2) * Matrix4::translation({ 2, 3, 5 })) == Approx{ 4.f * 4.f * 4.f * 2.f * 2.f * 2.f }.epsilon(2e-5));
}

TEST_CASE("matrix4.euler")
{
	const Matrix4 actual{ Yt::Euler{ 30, 45, 60 } };
	const auto expected = Matrix4::rotation(30, { 0, 0, -1 }) * Matrix4::rotation(45, { 1, 0, 0 }) * Matrix4::rotation(60, { 0, 1, 0 });
	CHECK(actual.x.x == Approx{ expected.x.x }.epsilon(3e-5));
	CHECK(actual.x.y == Approx{ expected.x.y }.epsilon(3e-5));
	CHECK(actual.x.z == Approx{ expected.x.z }.epsilon(3e-5));
	CHECK(actual.x.w == Approx{ expected.x.w }.epsilon(3e-5));
	CHECK(actual.y.x == Approx{ expected.y.x }.epsilon(3e-5));
	CHECK(actual.y.y == Approx{ expected.y.y }.epsilon(3e-5));
	CHECK(actual.y.z == Approx{ expected.y.z }.epsilon(3e-5));
	CHECK(actual.y.w == Approx{ expected.y.w }.epsilon(3e-5));
	CHECK(actual.z.x == Approx{ expected.z.x }.epsilon(3e-5));
	CHECK(actual.z.y == Approx{ expected.z.y }.epsilon(3e-5));
	CHECK(actual.z.z == Approx{ expected.z.z }.epsilon(3e-5));
	CHECK(actual.z.w == Approx{ expected.z.w }.epsilon(3e-5));
	CHECK(actual.t.x == Approx{ expected.t.x }.epsilon(3e-5));
	CHECK(actual.t.y == Approx{ expected.t.y }.epsilon(3e-5));
	CHECK(actual.t.z == Approx{ expected.t.z }.epsilon(3e-5));
	CHECK(actual.t.w == Approx{ expected.t.w }.epsilon(3e-5));
}

TEST_CASE("matrix4.inverse")
{
	CHECK(inverse(Matrix4::identity()) == Matrix4::identity());
	CHECK(inverse(Matrix4::translation({ 2, 3, 5 })) == Matrix4::translation({ -2, -3, -5 }));
	CHECK(inverse(Matrix4::scaling(4)) == Matrix4::scaling(0.25));
	{
		const auto actual = inverse(Matrix4::rotation(37, { 2, 3, 5 }));
		const auto expected = Matrix4::rotation(-37, { 2, 3, 5 });
		CHECK(actual.x.x == Approx{ expected.x.x }.epsilon(2e-5));
		CHECK(actual.x.y == Approx{ expected.x.y }.epsilon(2e-5));
		CHECK(actual.x.z == Approx{ expected.x.z }.epsilon(2e-5));
		CHECK(actual.x.w == Approx{ expected.x.w }.epsilon(2e-5));
		CHECK(actual.y.x == Approx{ expected.y.x }.epsilon(2e-5));
		CHECK(actual.y.y == Approx{ expected.y.y }.epsilon(2e-5));
		CHECK(actual.y.z == Approx{ expected.y.z }.epsilon(2e-5));
		CHECK(actual.y.w == Approx{ expected.y.w }.epsilon(2e-5));
		CHECK(actual.z.x == Approx{ expected.z.x }.epsilon(2e-5));
		CHECK(actual.z.y == Approx{ expected.z.y }.epsilon(2e-5));
		CHECK(actual.z.z == Approx{ expected.z.z }.epsilon(2e-5));
		CHECK(actual.z.w == Approx{ expected.z.w }.epsilon(2e-5));
		CHECK(actual.t.x == Approx{ expected.t.x }.epsilon(2e-5));
		CHECK(actual.t.y == Approx{ expected.t.y }.epsilon(2e-5));
		CHECK(actual.t.z == Approx{ expected.t.z }.epsilon(2e-5));
		CHECK(actual.t.w == Approx{ expected.t.w }.epsilon(2e-5));
	}
	{
		const auto actual = inverse(Matrix4::scaling(4) * Matrix4::rotation(37, { 2, 3, 5 }) * Matrix4::scaling(2) * Matrix4::translation({ 2, 3, 5 }));
		const auto expected = Matrix4::translation({ -2, -3, -5 }) * Matrix4::scaling(0.5) * Matrix4::rotation(-37, { 2, 3, 5 }) * Matrix4::scaling(0.25);
		CHECK(actual.x.x == Approx{ expected.x.x }.epsilon(2e-5));
		CHECK(actual.x.y == Approx{ expected.x.y }.epsilon(2e-5));
		CHECK(actual.x.z == Approx{ expected.x.z }.epsilon(2e-5));
		CHECK(actual.x.w == Approx{ expected.x.w }.epsilon(2e-5));
		CHECK(actual.y.x == Approx{ expected.y.x }.epsilon(2e-5));
		CHECK(actual.y.y == Approx{ expected.y.y }.epsilon(2e-5));
		CHECK(actual.y.z == Approx{ expected.y.z }.epsilon(2e-5));
		CHECK(actual.y.w == Approx{ expected.y.w }.epsilon(2e-5));
		CHECK(actual.z.x == Approx{ expected.z.x }.epsilon(2e-5));
		CHECK(actual.z.y == Approx{ expected.z.y }.epsilon(2e-5));
		CHECK(actual.z.z == Approx{ expected.z.z }.epsilon(2e-5));
		CHECK(actual.z.w == Approx{ expected.z.w }.epsilon(2e-5));
		CHECK(actual.t.x == Approx{ expected.t.x }.epsilon(2e-5));
		CHECK(actual.t.y == Approx{ expected.t.y }.epsilon(2e-5));
		CHECK(actual.t.z == Approx{ expected.t.z }.epsilon(2e-5));
		CHECK(actual.t.w == Approx{ expected.t.w }.epsilon(2e-5));
	}
}

TEST_CASE("matrix4.perspective")
{
	using Yt::Vector3;

	const auto m = Matrix4::perspective({ 1, 1 }, 90, 1, 2);
	{
		const auto v = m * Vector3{ -1, -1, -1 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ -2, -2, -2 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ -1, 1, -1 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ -2, 2, -2 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 1, -1, -1 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 2, -2, -2 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 1, 1, -1 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 2, 2, -2 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
}

TEST_CASE("matrix4.projection_2d")
{
	using Yt::Vector3;

	const auto m = Matrix4::projection_2d({ 640, 480 }, -.25, .75);
	{
		const auto v = m * Vector3{ 0, 0, -.25 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 0, 0, .75 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 640, 0, -.25 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 640, 0, .75 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 0, 480, -.25 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 0, 480, .75 };
		CHECK(v.x == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 640, 480, -.25 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ 1.f }.epsilon(2e-5));
	}
	{
		const auto v = m * Vector3{ 640, 480, .75 };
		CHECK(v.x == Approx{ 1.f }.epsilon(2e-5));
		CHECK(v.y == Approx{ -1.f }.epsilon(2e-5));
		CHECK(v.z == Approx{ -1.f }.epsilon(2e-5));
	}
}
