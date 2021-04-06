// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "utils.h"

#include <cstring>

namespace Yt
{
	void copy_image_rgb_bgr(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 3;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 3, b += 3)
			{
				dst[a + 0] = src[b + 2];
				dst[a + 1] = src[b + 1];
				dst[a + 2] = src[b + 0];
			}
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_rgb_bgra(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 3)
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

	void copy_image_rgb_rgba(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 3)
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

	void copy_image_rgba_bgra(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 4)
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

	void copy_image_x_x(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		for (auto y = height; y > 0; --y)
		{
			std::memcpy(dst, src, width);
			src += src_stride;
			dst += dst_stride;
		}
	}

	void copy_image_x_xxxa(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 4, ++b)
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

	void copy_image_x_xxxx(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 4, ++b)
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

	void copy_image_xa_xxxa(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept
	{
		const auto dst_row_size = width * 4;
		for (auto y = height; y > 0; --y)
		{
			for (size_t a = 0, b = 0; a < dst_row_size; a += 4, b += 2)
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
}
