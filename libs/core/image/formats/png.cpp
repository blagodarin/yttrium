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
#include "../formats.h"

#include <limits>

#include <png.h>

namespace
{
	bool can_write(const Yttrium::ImageInfo& info)
	{
		switch (info.pixel_format())
		{
		case Yttrium::PixelFormat::Gray8:
		case Yttrium::PixelFormat::GrayAlpha16:
		case Yttrium::PixelFormat::Rgb24:
		case Yttrium::PixelFormat::Bgr24:
		case Yttrium::PixelFormat::Rgba32:
		case Yttrium::PixelFormat::Bgra32:
			break;
		default:
			return false;
		}

		if (info.orientation() != Yttrium::ImageOrientation::XRightYDown && info.orientation() != Yttrium::ImageOrientation::XRightYUp)
			return false;

		if (info.width() <= 0 || info.width() > std::numeric_limits<std::uint32_t>::max())
			return false;

		if (info.height() <= 0 || info.height() > std::numeric_limits<std::uint32_t>::max())
			return false;

		return true;
	}

	class PngWriter
	{
	public:
		explicit PngWriter(Yttrium::Writer& writer)
		{
			_png = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
			if (!_png)
				return;
			_info = ::png_create_info_struct(_png);
			if (!_info)
				return;
			::png_set_write_fn(_png, &writer, write_callback, flush_callback);
		}

		~PngWriter()
		{
			if (_png)
			{
				if (_info)
					::png_destroy_write_struct(&_png, &_info);
				else
					::png_destroy_write_struct(&_png, nullptr);
			}
		}

		// Code with 'setjmp' must reside in a separate function
		// without local variables to avoid clobbering warnings.
		bool write(const Yttrium::ImageInfo& info, int color_type, int bits_per_channel, int transforms, png_bytep* data)
		{
			if (setjmp(png_jmpbuf(_png)))
				return false;

			::png_set_compression_level(_png, 0);
			::png_set_IHDR(_png, _info, static_cast<std::uint32_t>(info.width()), static_cast<std::uint32_t>(info.height()),
				bits_per_channel, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

			::png_set_rows(_png, _info, data);
			::png_write_png(_png, _info, transforms, nullptr);

			return true;
		}

	private:
		static void write_callback(png_struct* png_ptr, png_byte* data, png_size_t length)
		{
			reinterpret_cast<Yttrium::Writer*>(::png_get_io_ptr(png_ptr))->write(data, length);
		}

		static void flush_callback(png_struct*)
		{
		}

	public:
		png_struct* _png = nullptr;
		png_info* _info = nullptr;
	};
}

namespace Yttrium
{
	bool write_png(Writer& writer, const ImageInfo& info, const void* data)
	{
		if (!::can_write(info))
			return false;

		int color_type = 0;
		int bits_per_channel = 0;
		int transforms = 0;

		if (info.pixel_format() == PixelFormat::Bgr24 || info.pixel_format() == PixelFormat::Bgra32)
			transforms |= PNG_TRANSFORM_BGR;

		switch (info.pixel_format())
		{
		case PixelFormat::Gray8:
			color_type = PNG_COLOR_TYPE_GRAY;
			bits_per_channel = 8;
			break;

		case PixelFormat::GrayAlpha16:
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
			bits_per_channel = 8;
			break;

		case PixelFormat::Rgb24:
		case PixelFormat::Bgr24:
			color_type = PNG_COLOR_TYPE_RGB;
			bits_per_channel = 8;
			break;

		case PixelFormat::Rgba32:
		case PixelFormat::Bgra32:
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			bits_per_channel = 8;
			break;
		}

		const auto rows = std::make_unique<png_bytep[]>(info.height());
		if (info.orientation() == ImageOrientation::XRightYDown)
			for (std::size_t i = 0, j = 0; i < info.height(); i++, j += info.row_size())
				rows[i] = static_cast<png_bytep>(const_cast<void*>(data)) + j;
		else
			for (std::size_t i = info.height(), j = 0; i > 0; i--, j += info.row_size())
				rows[i - 1] = static_cast<png_bytep>(const_cast<void*>(data)) + j;

		return PngWriter(writer).write(info, color_type, bits_per_channel, transforms, rows.get());
	}
}
