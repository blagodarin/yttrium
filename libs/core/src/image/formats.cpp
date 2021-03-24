// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "formats.h"

#include <yttrium/image/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include <yttrium/utils/numeric.h>

#include <algorithm>
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
#if YTTRIUM_WITH_JPEG
		case ImageFormat::Jpeg: {
			auto input = source.to_buffer();
			return read_jpeg(input.data(), input.size(), info, buffer);
		}
#endif
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
		return buffer.try_reset(frame_size)
			&& reader.read(buffer.data(), frame_size) == frame_size;
	}

	bool write_image(Writer&& writer, ImageFormat format, int quality, const ImageInfo& info, const void* data)
	{
		quality = std::clamp(quality, 0, 100);
		switch (format)
		{
		case ImageFormat::Tga: return write_tga(writer, info, data);
#if YTTRIUM_WITH_JPEG
		case ImageFormat::Jpeg: return write_jpeg(writer, info, data, quality);
#endif
		case ImageFormat::Png: return write_png(writer, info, data);
		default: return false;
		}
	}
}
