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
	constexpr std::size_t aligned_image_row_size(std::size_t width, std::size_t pixel_size, std::size_t row_alignment)
	{
		return (width * pixel_size + row_alignment - 1) / row_alignment * row_alignment;
	}
}

namespace Yttrium
{
	ImageInfo::ImageInfo(std::size_t width, std::size_t height, PixelFormat pixel_format, std::size_t row_alignment, ImageOrientation orientation)
		: _pixel_format{ pixel_format }
		, _orientation{ orientation }
		, _width{ width }
		, _row_alignment{ row_alignment }
		, _row_size{ ::aligned_image_row_size(_width, pixel_size(_pixel_format), _row_alignment) }
		, _height{ height }
	{
		assert(is_power_of_2(_row_alignment));
	}

	std::optional<Image> Image::load(const Source& source, ImageType type)
	{
		if (type == ImageType::Auto && !detect_image_type(source, type))
			return {};
		Buffer buffer;
		const auto info = read_image(source, type, buffer);
		if (!info)
			return {};
		return Image{ *info, std::move(buffer) };
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
		const auto row_size = _info.row_size();
		Buffer row_buffer{ row_size };
		for (auto top = &_buffer[0], bottom = &_buffer[(_info.height() - 1) * row_size]; top < bottom; top += row_size, bottom -= row_size)
		{
			std::memcpy(row_buffer.data(), top, row_size);
			std::memcpy(top, bottom, row_size);
			std::memcpy(bottom, row_buffer.data(), row_size);
		}
		switch (_info._orientation)
		{
		case ImageOrientation::XRightYDown: _info._orientation = ImageOrientation::XRightYUp; break;
		case ImageOrientation::XRightYUp: _info._orientation = ImageOrientation::XRightYDown; break;
		case ImageOrientation::XLeftYDown: _info._orientation = ImageOrientation::XLeftYUp; break;
		case ImageOrientation::XLeftYUp: _info._orientation = ImageOrientation::XLeftYDown; break;
		}
	}

	bool Image::save(Writer&& writer, ImageType type) const
	{
		return write_image(writer, type, _info, _buffer.data());
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
				scanline += _info.row_size();
			}
		}
			_info._pixel_format = _info.pixel_format() == PixelFormat::Rgb24 ? PixelFormat::Bgr24 : PixelFormat::Rgb24;
			return true;

		default:
			break;
		}

		return false;
	}

	Buffer Image::to_buffer(ImageType type) const
	{
		Buffer buffer;
		return save(Writer{ buffer }, type) ? std::move(buffer) : Buffer{};
	}
}
