//
// Copyright 2018 Sergei Blagodarin
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
	bool read_image_data(Yttrium::Reader& reader, const Yttrium::ImageFormat& format, Yttrium::Buffer& buffer)
	{
		const auto frame_size = format.frame_size();
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
	std::optional<ImageFormat> read_image(const Source& source, ImageType type, Buffer& buffer)
	{
		Reader reader{ source };
		std::optional<ImageFormat> format;
		switch (type)
		{
		case ImageType::Tga: format = read_tga_header(reader); break;
#if Y_USE_JPEG
		case ImageType::Jpeg: return read_jpeg(source, buffer);
#endif
		case ImageType::Dds: format = read_dds_header(reader); break;
		default: return {};
		}
		if (!format || !read_image_data(reader, *format, buffer))
			return {};
		return format;
	}

	bool write_image(Writer& writer, ImageType type, const ImageFormat& format, const void* data)
	{
		switch (type)
		{
		case ImageType::Tga: return write_tga(writer, format, data);
#if Y_USE_PNG
		case ImageType::Png: return write_png(writer, format, data);
#endif
		default: return false;
		}
	}

	bool detect_image_type(const Source& source, ImageType& type)
	{
		std::uint16_t signature = 0;
		if (!source.read_at(0, signature))
			return false;
		switch (signature)
		{
		case "BM"_twocc:
			type = ImageType::Bmp;
			return true;
		case "DD"_twocc:
			type = ImageType::Dds;
			return true;
#if Y_USE_JPEG
		case "\xff\xd8"_twocc: // SOI marker.
			type = ImageType::Jpeg;
			return true;
#endif
#if Y_USE_PNG
		case "\x89P"_twocc:
			type = ImageType::Png;
			return true;
#endif
		default:
			// TGA is the last remaining option, and TGA images have no signature,
			// so we need to read the entire header anyway to determine whether it
			// is actually a TGA image.
			type = ImageType::Tga;
			return true;
		}
	}
}
