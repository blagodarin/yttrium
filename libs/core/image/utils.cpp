//
// Copyright 2019 Sergei Blagodarin
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

#include "utils.h"

#include <yttrium/utils/numeric.h>

#include <cstring>

namespace
{
	struct YCbCrLookupTables
	{
		int _cb_b[256];
		int _cb_g[256];
		int _cr_g[256];
		int _cr_r[256];

		static constexpr int Scale = 16;

		YCbCrLookupTables() noexcept
		{
			constexpr auto OneHalf = 1 << (Scale - 1);
			for (int i = 0; i < 256; ++i)
			{
				const auto k = i - 128;
				_cb_b[i] = (fixed(1.77200f) * k + OneHalf) >> Scale;
				_cb_g[i] = (-fixed(0.34414f)) * k + OneHalf; // Scaled during conversion.
				_cr_g[i] = (-fixed(0.71414f)) * k; // Added to the corresponding Cb-to-G value which already includes OneHalf.
				_cr_r[i] = (fixed(1.40200f) * k + OneHalf) >> Scale;
			}
		}

	private:
		static constexpr int fixed(float x) noexcept
		{
			return Yttrium::float_to_fixed<int, Scale>(x);
		}
	};

	const YCbCrLookupTables _ycbcr;
}

namespace Yttrium
{
	void copy_image_rgb_bgr(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 3;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 3, b += 3)
			{
				dst[a + 0] = src[b + 2];
				dst[a + 1] = src[b + 1];
				dst[a + 2] = src[b + 0];
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_rgb_bgra(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 3)
			{
				dst[a + 0] = src[b + 2];
				dst[a + 1] = src[b + 1];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = 0xff;
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_rgb_rgba(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 3)
			{
				dst[a + 0] = src[b + 0];
				dst[a + 1] = src[b + 1];
				dst[a + 2] = src[b + 2];
				dst[a + 3] = 0xff;
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_rgba_bgra(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 4)
			{
				dst[a + 0] = src[b + 2];
				dst[a + 1] = src[b + 1];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = src[b + 3];
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_x_x(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		for (auto y = height; y > 0; --y)
		{
			std::memcpy(dst, src, width);
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_x_xxxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 4, ++b)
			{
				dst[a + 0] = src[b + 0];
				dst[a + 1] = src[b + 0];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = 0xff;
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_x_xxxx(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 4, ++b)
			{
				dst[a + 0] = src[b + 0];
				dst[a + 1] = src[b + 0];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = src[b + 0];
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_xa_xxxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (std::size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 2)
			{
				dst[a + 0] = src[b + 0];
				dst[a + 1] = src[b + 0];
				dst[a + 2] = src[b + 0];
				dst[a + 3] = src[b + 1];
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void convert_jpeg420_bgra(std::size_t width, std::size_t height, const YCbCrComponents& src, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		auto y1 = src.y;
		auto y2 = src.y + src.y_stride;
		const auto y_delta = src.y_stride * 2 - width;
		auto cb = src.cb;
		auto cr = src.cr;
		const auto cbcr_delta = src.cbcr_stride - width / 2;
		auto bgra1 = dst;
		auto bgra2 = dst + dst_stride;
		const auto bgra_delta = dst_stride * 2 - width * 4;
		for (auto j = height / 2; j > 0; --j)
		{
			for (auto i = width / 2; i > 0; --i)
			{
				const auto bx = _ycbcr._cb_b[*cb];
				const auto gx = (_ycbcr._cb_g[*cb] + _ycbcr._cr_g[*cr]) >> YCbCrLookupTables::Scale;
				const auto rx = _ycbcr._cr_r[*cr];
				bgra1[0] = clamp_to_uint8(y1[0] + bx);
				bgra1[1] = clamp_to_uint8(y1[0] + gx);
				bgra1[2] = clamp_to_uint8(y1[0] + rx);
				bgra1[3] = 255;
				bgra1[4] = clamp_to_uint8(y1[1] + bx);
				bgra1[5] = clamp_to_uint8(y1[1] + gx);
				bgra1[6] = clamp_to_uint8(y1[1] + rx);
				bgra1[7] = 255;
				bgra2[0] = clamp_to_uint8(y2[0] + bx);
				bgra2[1] = clamp_to_uint8(y2[0] + gx);
				bgra2[2] = clamp_to_uint8(y2[0] + rx);
				bgra2[3] = 255;
				bgra2[4] = clamp_to_uint8(y2[1] + bx);
				bgra2[5] = clamp_to_uint8(y2[1] + gx);
				bgra2[6] = clamp_to_uint8(y2[1] + rx);
				bgra2[7] = 255;
				y1 += 2;
				y2 += 2;
				++cb;
				++cr;
				bgra1 += 8;
				bgra2 += 8;
			}
			y1 += y_delta;
			y2 += y_delta;
			cb += cbcr_delta;
			cr += cbcr_delta;
			bgra1 += bgra_delta;
			bgra2 += bgra_delta;
		}
	}
}
