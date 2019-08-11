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

#include <yttrium/api.h>

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
