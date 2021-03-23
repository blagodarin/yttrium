// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>

namespace Yt
{
	void copy_image_rgb_bgr(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_rgb_bgra(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_rgb_rgba(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_rgba_bgra(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_x_x(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_x_xxxa(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_x_xxxx(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
	void copy_image_xa_xxxa(size_t width, size_t height, const uint8_t* src, ptrdiff_t src_stride, uint8_t* dst, ptrdiff_t dst_stride) noexcept;
}
