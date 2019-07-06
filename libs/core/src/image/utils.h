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

namespace Yttrium
{
	void copy_image_rgb_bgr(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_rgb_bgra(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_rgb_rgba(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_rgba_bgra(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_x_x(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_x_xxxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_x_xxxx(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;
	void copy_image_xa_xxxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept;

	struct YCbCrComponents
	{
		const std::uint8_t* y;
		std::size_t y_stride;
		const std::uint8_t* cb;
		std::size_t cbcr_stride;
		const std::uint8_t* cr;
	};

	Y_CORE_API void convert_jpeg420_to_bgra(std::size_t width, std::size_t height, const YCbCrComponents&, void* dst, std::size_t dst_stride) noexcept;
	void convert_jpeg444_to_bgra(std::size_t width, std::size_t height, const YCbCrComponents&, void* dst, std::size_t dst_stride) noexcept;

	void upsample_2x2_linear(std::size_t width, std::size_t height, const std::uint8_t* src, std::size_t src_stride, std::uint8_t* dst, std::size_t dst_stride) noexcept;
}
