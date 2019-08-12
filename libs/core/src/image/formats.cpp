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

#include "formats.h"

#include <yttrium/image/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/utils/numeric.h>

#include <new>

namespace Yt
{
	bool read_image(const Source& source, ImageFormat format, ImageInfo& info, Buffer& buffer)
	{
		if (format == ImageFormat::Auto)
		{
			struct
			{
				uint16_t ab = 0;
				uint16_t cd = 0;
			} signature;
			static_assert(sizeof signature == 4);
			if (!source.read_at(0, signature))
				return false;
			switch (signature.ab)
			{
			case make_cc('B', 'M'):
				format = ImageFormat::Bmp;
				break;
			case make_cc('D', 'D'):
				format = ImageFormat::Dds;
				break;
			case make_cc('\xff', '\xd8'): // SOI marker.
				format = ImageFormat::Jpeg;
				break;
			case make_cc('\x89', 'P'):
				format = ImageFormat::Png;
				break;
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
			}
		}

		Reader reader{ source };
		switch (format)
		{
		case ImageFormat::Tga:
			if (!read_tga_header(reader, info))
				return false;
			break;
		case ImageFormat::Jpeg:
		{
			auto input = source.to_buffer();
			return read_jpeg(input.data(), input.size(), info, buffer);
		}
		case ImageFormat::Dds:
			if (!read_dds_header(reader, info))
				return false;
			break;
		case ImageFormat::Bmp:
			if (!read_bmp_header(reader, info))
				return false;
			break;
		case ImageFormat::Ico:
			if (!read_ico_header(reader, info))
				return false;
			break;
		default:
			return false;
		}

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

	bool write_image(Writer& writer, ImageFormat format, const ImageInfo& info, const void* data)
	{
		switch (format)
		{
		case ImageFormat::Tga: return write_tga(writer, info, data);
		case ImageFormat::Jpeg: return write_jpeg(writer, info, data);
		case ImageFormat::Png: return write_png(writer, info, data);
		default: return false;
		}
	}
}
