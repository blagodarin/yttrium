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

#include "formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "../utils/fourcc.h"

#include <new>

namespace
{
	bool read_image_data(Yttrium::Reader& reader, const Yttrium::ImageInfo& info, Yttrium::Buffer& buffer)
	{
		const auto frame_size = info.frame_size();
		try
		{
			buffer.reset(frame_size);
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
		return reader.read(buffer.data(), frame_size) == frame_size;
	}
}

namespace Yttrium
{
	std::optional<ImageInfo> read_image(const Source& source, ImageFormat format, Buffer& buffer)
	{
		Reader reader{ source };
		std::optional<ImageInfo> info;
		switch (format)
		{
		case ImageFormat::Tga: info = read_tga_header(reader); break;
#if Y_USE_JPEG
		case ImageFormat::Jpeg: return read_jpeg(source, buffer);
#endif
		case ImageFormat::Dds: info = read_dds_header(reader); break;
		case ImageFormat::Bmp: info = read_bmp_header(reader); break;
		case ImageFormat::Ico: info = read_ico_header(reader); break;
		default: return {};
		}
		if (!info || !read_image_data(reader, *info, buffer))
			return {};
		return info;
	}

	bool write_image(Writer& writer, ImageFormat format, const ImageInfo& info, const void* data)
	{
		switch (format)
		{
		case ImageFormat::Tga: return write_tga(writer, info, data);
#if Y_USE_PNG
		case ImageFormat::Png: return write_png(writer, info, data);
#endif
		default: return false;
		}
	}

	bool detect_image_format(const Source& source, ImageFormat& format)
	{
		struct
		{
			std::uint16_t ab = 0;
			std::uint16_t cd = 0;
		} signature;
		static_assert(sizeof signature == 4);
		if (!source.read_at(0, signature))
			return false;
		switch (signature.ab)
		{
		case "BM"_twocc:
			format = ImageFormat::Bmp;
			return true;
		case "DD"_twocc:
			format = ImageFormat::Dds;
			return true;
		case "\xff\xd8"_twocc: // SOI marker.
#if Y_USE_JPEG
			format = ImageFormat::Jpeg;
			return true;
#else
			return false;
#endif
		case "\x89P"_twocc:
#if Y_USE_PNG
			format = ImageFormat::Png;
			return true;
#else
			return false;
#endif
		default:
			if (signature.ab)
			{
				// ICO: reserved value, must be zero.
				// TGA: ID length and color map type, may be non-zero.
				format = ImageFormat::Tga;
			}
			else if (signature.cd == 1)
			{
				// ICO: file type, 1 for ICO file.
				// TGA: image type and lower byte of colormap description, 1 means color-mapped image (unsupported).
				format = ImageFormat::Ico;
			}
			else
				format = ImageFormat::Tga; // We're still unsure this isn't TGA.
			return true;
		}
	}
}
