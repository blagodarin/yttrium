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

	void Image::flip_vertically()
	{
		const auto stride = _info.stride();
		Buffer row_buffer{ stride };
		for (auto top = &_buffer[0], bottom = &_buffer[(_info.height() - 1) * stride]; top < bottom; top += stride, bottom -= stride)
		{
			std::memcpy(row_buffer.data(), top, stride);
			std::memcpy(top, bottom, stride);
			std::memcpy(bottom, row_buffer.data(), stride);
		}
		_info._orientation = _info._orientation == ImageOrientation::XRightYDown ? ImageOrientation::XRightYUp : ImageOrientation::XRightYDown;
	}

	bool Image::save(Writer&& writer, ImageFormat format) const
	{
		return write_image(writer, format, _info, _buffer.data());
	}

	bool Image::swap_channels() noexcept
	{
		switch (_info.pixel_format())
		{
		case PixelFormat::Gray8:
			return true;

		case PixelFormat::Rgb24:
		case PixelFormat::Bgr24:
		{
			auto scanline = static_cast<uint8_t*>(_buffer.data());
			for (std::size_t row = 0; row < _info.height(); ++row)
			{
				for (std::size_t offset = 0; offset < _info.width() * 3; offset += 3)
				{
					const auto x = scanline[offset];
					scanline[offset] = scanline[offset + 2];
					scanline[offset + 2] = x;
				}
				scanline += _info.stride();
			}
		}
			_info._pixel_format = _info.pixel_format() == PixelFormat::Rgb24 ? PixelFormat::Bgr24 : PixelFormat::Rgb24;
			return true;

		default:
			break;
		}

		return false;
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

		auto src = static_cast<const std::uint8_t*>(src_data);
		const auto src_stride = src_info.stride();

		auto dst = static_cast<std::uint8_t*>(dst_data);
		auto dst_stride = static_cast<std::ptrdiff_t>(dst_info.stride());
		if (src_info.orientation() != dst_info.orientation())
		{
			dst += (height - 1) * dst_stride;
			dst_stride = -dst_stride;
		}

		const auto dst_row_size = ImageInfo::stride(width, dst_info.pixel_format());

		if (src_info.pixel_format() == dst_info.pixel_format())
		{
			for (auto y = height; y > 0; --y)
			{
				std::memcpy(dst, src, dst_row_size);
				src += src_stride;
				dst += dst_stride;
			}
			return true;
		}

		if (dst_info.pixel_format() != PixelFormat::Bgra32)
			return false;

		switch (src_info.pixel_format())
		{
		case PixelFormat::Intensity8:
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
			break;

		case PixelFormat::Gray8:
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
			break;

		case PixelFormat::GrayAlpha16:
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
			break;

		case PixelFormat::Rgb24:
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
			break;

		case PixelFormat::Bgr24:
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
			break;

		case PixelFormat::Rgba32:
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
			break;

		default:
			return false;
		}

		return true;
	}
}
