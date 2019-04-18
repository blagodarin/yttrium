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

#if Y_ARCH_X86
#	ifdef _MSC_VER
#		include <intrin.h>
#	else
#		include <x86intrin.h>
#	endif
#endif

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

	void convert_jpeg420_to_bgra(std::size_t width, std::size_t height, const YCbCrComponents& src, void* dst, std::size_t dst_stride) noexcept
	{
		auto y1 = src.y;
		auto y2 = src.y + src.y_stride;
		const auto y_delta = src.y_stride * 2 - width;
		auto cb = src.cb;
		auto cr = src.cr;
		const auto cbcr_delta = src.cbcr_stride - width / 2;
		auto bgra1 = static_cast<std::uint8_t*>(dst);
		auto bgra2 = bgra1 + dst_stride;
		const auto bgra_delta = dst_stride * 2 - width * 4;
#if Y_ARCH_X86
		if (!(width & 0xf) && !(reinterpret_cast<std::uintptr_t>(dst) & 0xf) && !(dst_stride & 0xf))
		{
			const auto alpha = _mm_set1_epi16(255);
			const auto bias = _mm_set1_epi8(static_cast<char>(0x80));

			const auto cr_r = _mm_set1_epi16(fixed_point<std::int16_t, 12>(1.402));
			const auto cr_g = _mm_set1_epi16(-fixed_point<std::int16_t, 12>(0.7141363));
			const auto cb_g = _mm_set1_epi16(-fixed_point<std::int16_t, 12>(0.3441363));
			const auto cb_b = _mm_set1_epi16(fixed_point<std::int16_t, 12>(1.772));

			for (auto j = height / 2; j > 0; --j)
			{
				for (auto i = width / 8; i > 0; --i)
				{
					const auto y1_values = _mm_srli_epi16(_mm_unpacklo_epi8(bias, _mm_loadl_epi64(reinterpret_cast<const __m128i*>(y1))), 4);
					const auto y2_values = _mm_srli_epi16(_mm_unpacklo_epi8(bias, _mm_loadl_epi64(reinterpret_cast<const __m128i*>(y2))), 4);

					// TODO: Load 4 bytes.
					const auto cb_bytes = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(cb));
					const auto cr_bytes = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(cr));

					const auto cb_values = _mm_unpacklo_epi8(_mm_setzero_si128(), _mm_xor_si128(_mm_unpacklo_epi8(cb_bytes, cb_bytes), bias));
					const auto cr_values = _mm_unpacklo_epi8(_mm_setzero_si128(), _mm_xor_si128(_mm_unpacklo_epi8(cr_bytes, cr_bytes), bias));

					const auto xb_values = _mm_mulhi_epi16(cb_values, cb_b);
					const auto xg_values = _mm_add_epi16(_mm_mulhi_epi16(cb_values, cb_g), _mm_mulhi_epi16(cr_values, cr_g));
					const auto xr_values = _mm_mulhi_epi16(cr_values, cr_r);

					const auto br1 = _mm_packus_epi16(_mm_srai_epi16(_mm_add_epi16(y1_values, xb_values), 4), _mm_srai_epi16(_mm_add_epi16(y1_values, xr_values), 4));
					const auto br2 = _mm_packus_epi16(_mm_srai_epi16(_mm_add_epi16(y2_values, xb_values), 4), _mm_srai_epi16(_mm_add_epi16(y2_values, xr_values), 4));
					const auto ga1 = _mm_packus_epi16(_mm_srai_epi16(_mm_add_epi16(y1_values, xg_values), 4), alpha);
					const auto ga2 = _mm_packus_epi16(_mm_srai_epi16(_mm_add_epi16(y2_values, xg_values), 4), alpha);

					const auto bg1 = _mm_unpacklo_epi8(br1, ga1);
					const auto bg2 = _mm_unpacklo_epi8(br2, ga2);
					const auto ra1 = _mm_unpackhi_epi8(br1, ga1);
					const auto ra2 = _mm_unpackhi_epi8(br2, ga2);

					_mm_store_si128(reinterpret_cast<__m128i*>(bgra1) + 0, _mm_unpacklo_epi16(bg1, ra1));
					_mm_store_si128(reinterpret_cast<__m128i*>(bgra1) + 1, _mm_unpackhi_epi16(bg1, ra1));
					_mm_store_si128(reinterpret_cast<__m128i*>(bgra2) + 0, _mm_unpacklo_epi16(bg2, ra2));
					_mm_store_si128(reinterpret_cast<__m128i*>(bgra2) + 1, _mm_unpackhi_epi16(bg2, ra2));

					y1 += 8;
					y2 += 8;
					cb += 4;
					cr += 4;
					bgra1 += 32;
					bgra2 += 32;
				}
				y1 += y_delta;
				y2 += y_delta;
				cb += cbcr_delta;
				cr += cbcr_delta;
				bgra1 += bgra_delta;
				bgra2 += bgra_delta;
			}
			return;
		}
#endif
		for (auto j = height / 2; j > 0; --j)
		{
			for (auto i = width / 2; i > 0; --i)
			{
				// See ITU-T T.871 for the exact equations.
				const auto xcb = *cb - 128;
				const auto xcr = *cr - 128;
				const auto xb = ((1 << (16 - 1)) + fixed_point<int, 16>(1.772) * xcb) >> 16;
				const auto xg = ((1 << (16 - 1)) - fixed_point<int, 16>(0.3441363) * xcb - fixed_point<int, 16>(0.7141363) * xcr) >> 16;
				const auto xr = ((1 << (16 - 1)) + fixed_point<int, 16>(1.402) * xcr) >> 16;
				bgra1[0] = clamp_to_uint8(y1[0] + xb);
				bgra1[1] = clamp_to_uint8(y1[0] + xg);
				bgra1[2] = clamp_to_uint8(y1[0] + xr);
				bgra1[3] = 255;
				bgra1[4] = clamp_to_uint8(y1[1] + xb);
				bgra1[5] = clamp_to_uint8(y1[1] + xg);
				bgra1[6] = clamp_to_uint8(y1[1] + xr);
				bgra1[7] = 255;
				bgra2[0] = clamp_to_uint8(y2[0] + xb);
				bgra2[1] = clamp_to_uint8(y2[0] + xg);
				bgra2[2] = clamp_to_uint8(y2[0] + xr);
				bgra2[3] = 255;
				bgra2[4] = clamp_to_uint8(y2[1] + xb);
				bgra2[5] = clamp_to_uint8(y2[1] + xg);
				bgra2[6] = clamp_to_uint8(y2[1] + xr);
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
