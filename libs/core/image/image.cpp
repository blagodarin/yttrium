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

#include <yttrium/image.h>

#include <yttrium/storage/writer.h>
#include <yttrium/utils/numeric.h>
#include "../utils/string.h"
#include "formats.h"

#include <cassert>
#include <cstring>

// TODO: Compressed images (e. g. compressed textures).
// TODO: Multi-level images (e. g. textures with mipmaps).
// TODO: Separate image header/data loading.
// TODO: Image packs (lists of image headers with filenames and raw image data offsets).
// TODO: Loading image data into the specified buffer (e. g. into mapped texture memory).

namespace
{
	void convert_x_to_x(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
	{
		for (auto y = height; y > 0; --y)
		{
			std::memcpy(dst, src, width);
			src += src_stride;
			dst += dst_stride;
		}
	}

	void convert_x_to_xxxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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

	void convert_x_to_xxxx(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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

	void convert_xa_to_xxxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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

	void convert_xyz_to_xyza(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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

	void convert_xyz_to_zyx(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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

	void convert_xyz_to_zyxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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

	void convert_xyza_to_zyxa(std::size_t width, std::size_t height, const std::uint8_t* src, std::ptrdiff_t src_stride, std::uint8_t* dst, std::ptrdiff_t dst_stride) noexcept
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
}

namespace Yttrium
{
	std::optional<Image> Image::load(const Source& source, ImageFormat format)
	{
		ImageInfo info;
		Buffer buffer;
		if (!read_image(source, format, info, buffer))
			return {};
		return Image{ info, std::move(buffer) };
	}

	Image::Image(const ImageInfo& info)
		: _info{ info }
		, _buffer{ _info.frame_size() }
	{
	}

	Image::Image(const ImageInfo& info, const void* data)
		: _info{ info }
		, _buffer{ _info.frame_size(), data }
	{
	}

	bool Image::save(Writer&& writer, ImageFormat format) const
	{
		return write_image(writer, format, _info, _buffer.data());
	}

	Buffer Image::to_buffer(ImageFormat format) const
	{
		Buffer buffer;
		return save(Writer{ buffer }, format) ? std::move(buffer) : Buffer{};
	}

	bool Image::transform(const ImageInfo& src_info, const void* src_data, const ImageInfo& dst_info, void* dst_data) noexcept
	{
		const auto width = src_info.width();
		const auto height = src_info.height();
		if (width != dst_info.width() || height != dst_info.height())
			return false;

		const auto src = static_cast<const std::uint8_t*>(src_data);
		const auto src_stride = src_info.stride();
		const auto src_format = src_info.pixel_format();

		auto dst = static_cast<std::uint8_t*>(dst_data);
		auto dst_stride = static_cast<std::ptrdiff_t>(dst_info.stride());
		const auto dst_format = dst_info.pixel_format();

		if (src_info.orientation() != dst_info.orientation())
		{
			dst += static_cast<std::ptrdiff_t>(height - 1) * dst_stride;
			dst_stride = -dst_stride;
		}

		if (src_format == dst_format)
		{
			::convert_x_to_x(ImageInfo::stride(width, dst_format), height, src, src_stride, dst, dst_stride);
			return true;
		}

		switch (src_format)
		{
		case PixelFormat::Intensity8:
			if (dst_format == PixelFormat::Bgra32 || dst_format == PixelFormat::Rgba32)
				::convert_x_to_xxxx(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		case PixelFormat::Gray8:
			if (dst_format == PixelFormat::Bgra32 || dst_format == PixelFormat::Rgba32)
				::convert_x_to_xxxa(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		case PixelFormat::GrayAlpha16:
			if (dst_format == PixelFormat::Bgra32 || dst_format == PixelFormat::Rgba32)
				::convert_xa_to_xxxa(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		case PixelFormat::Rgb24:
			if (dst_format == PixelFormat::Bgra32)
				::convert_xyz_to_zyxa(width, height, src, src_stride, dst, dst_stride);
			else if (dst_format == PixelFormat::Rgba32)
				::convert_xyz_to_xyza(width, height, src, src_stride, dst, dst_stride);
			else if (dst_format == PixelFormat::Bgr24)
				::convert_xyz_to_zyx(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		case PixelFormat::Bgr24:
			if (dst_format == PixelFormat::Bgra32)
				::convert_xyz_to_xyza(width, height, src, src_stride, dst, dst_stride);
			else if (dst_format == PixelFormat::Rgba32)
				::convert_xyz_to_zyxa(width, height, src, src_stride, dst, dst_stride);
			else if (dst_format == PixelFormat::Rgb24)
				::convert_xyz_to_zyx(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		case PixelFormat::Rgba32:
			if (dst_format == PixelFormat::Bgra32)
				::convert_xyza_to_zyxa(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		case PixelFormat::Bgra32:
			if (dst_format == PixelFormat::Rgba32)
				::convert_xyza_to_zyxa(width, height, src, src_stride, dst, dst_stride);
			else
				return false;
			break;

		default:
			return false;
		}

		return true;
	}
}
