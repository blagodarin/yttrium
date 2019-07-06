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

#include <yttrium/image.h>
#include <yttrium/storage/writer.h>
#include <yttrium/utils/numeric.h>
#include "../../utils/adler32.h"
#include "../../utils/crc32.h"
#include "../formats.h"

#include <limits>

namespace
{
	constexpr auto PngSignature = Yttrium::make_cc('\x89', 'P', 'N', 'G', '\r', '\n', '\x1a', '\n');

	enum class PngChunkType : uint32_t
	{
		IDAT = Yttrium::make_cc('I', 'D', 'A', 'T'),
		IEND = Yttrium::make_cc('I', 'E', 'N', 'D'),
		IHDR = Yttrium::make_cc('I', 'H', 'D', 'R'),
	};

	enum class PngColorType : uint8_t
	{
		Grayscale = 0,
		Truecolor = 2,
		Indexed = 3,
		GrayscaleAlpha = 4,
		TruecolorAlpha = 6,
	};

	enum class PngCompressionMethod : uint8_t
	{
		Zlib = 0,
	};

	enum class PngFilterMethod : uint8_t
	{
		Standard = 0,
	};

	enum class PngStandardFilterType : uint8_t
	{
		None = 0,
		Sub = 1,
		Up = 2,
		Average = 3,
		Paeth = 4,
	};

	enum class PngInterlaceMethod : uint8_t
	{
		None = 0,
		Adam7 = 1,
	};

#pragma pack(push, 1)

	struct PngPrefix
	{
		uint64_t signature;
		struct
		{
			uint32_t length;
			PngChunkType type;
			struct
			{
				uint32_t width;
				uint32_t height;
				uint8_t bit_depth;
				PngColorType color_type;
				PngCompressionMethod compression_method;
				PngFilterMethod filter_method;
				PngInterlaceMethod interlace_method;
			} data;
			uint32_t crc;
		} ihdr;
		struct
		{
			uint32_t length;
			PngChunkType type;
		} idat;
	};

	struct PngSuffix
	{
		struct
		{
			uint32_t crc;
		} idat;
		struct
		{
			uint32_t length;
			PngChunkType type;
			uint32_t crc;
		} iend;
	};

#pragma pack(pop)

	Yttrium::Buffer make_zlib_buffer(const void* data, size_t size)
	{
		constexpr size_t max_block_bytes = 65535;
		Yttrium::Buffer buffer;
		buffer.reserve(size + 2 + (size + max_block_bytes - 1) / max_block_bytes * 5 + 4);
		Yttrium::Writer writer{ buffer };
		writer.write(Yttrium::swap_bytes(uint16_t{ 0x7801 })); // Deflate algorithm, 32 KiB window, no compression.
		for (size_t offset = 0; offset < size;)
		{
			const auto remaining_bytes = size - offset;
			const auto block_bytes = remaining_bytes > max_block_bytes ? max_block_bytes : remaining_bytes;
			writer.write(static_cast<uint8_t>(block_bytes == remaining_bytes));
			writer.write(static_cast<uint16_t>(block_bytes));
			writer.write(static_cast<uint16_t>(~block_bytes));
			writer.write(static_cast<const std::byte*>(data) + offset, block_bytes);
			offset += block_bytes;
		}
		writer.write(Yttrium::swap_bytes(Yttrium::Adler32{}.process(data, size).value()));
		return buffer;
	}
}

namespace Yttrium
{
	bool write_png(Writer& writer, const ImageInfo& info, const void* data)
	{
		if (info.orientation() != ImageOrientation::XRightYDown && info.orientation() != ImageOrientation::XRightYUp)
			return false;

		if (!info.width() || info.width() > std::numeric_limits<uint32_t>::max())
			return false;

		if (!info.height() || info.height() > std::numeric_limits<uint32_t>::max())
			return false;

		PixelFormat pixel_format;

		PngPrefix prefix;
		prefix.signature = PngSignature;
		prefix.ihdr.length = swap_bytes(uint32_t{ sizeof prefix.ihdr.data });
		prefix.ihdr.type = PngChunkType::IHDR;
		prefix.ihdr.data.width = swap_bytes(static_cast<uint32_t>(info.width()));
		prefix.ihdr.data.height = swap_bytes(static_cast<uint32_t>(info.height()));
		prefix.ihdr.data.bit_depth = 8;
		switch (info.pixel_format())
		{
		case PixelFormat::Gray8:
			prefix.ihdr.data.color_type = PngColorType::Grayscale;
			pixel_format = PixelFormat::Gray8;
			break;
		case PixelFormat::GrayAlpha16:
			prefix.ihdr.data.color_type = PngColorType::GrayscaleAlpha;
			pixel_format = PixelFormat::GrayAlpha16;
			break;
		case PixelFormat::Rgb24:
			prefix.ihdr.data.color_type = PngColorType::Truecolor;
			pixel_format = PixelFormat::Rgb24;
			break;
		case PixelFormat::Bgr24:
			prefix.ihdr.data.color_type = PngColorType::Truecolor;
			pixel_format = PixelFormat::Rgb24;
			break;
		case PixelFormat::Rgba32:
			prefix.ihdr.data.color_type = PngColorType::TruecolorAlpha;
			pixel_format = PixelFormat::Rgba32;
			break;
		case PixelFormat::Bgra32:
			prefix.ihdr.data.color_type = PngColorType::TruecolorAlpha;
			pixel_format = PixelFormat::Rgba32;
			break;
		default:
			return false;
		}
		prefix.ihdr.data.compression_method = PngCompressionMethod::Zlib;
		prefix.ihdr.data.filter_method = PngFilterMethod::Standard;
		prefix.ihdr.data.interlace_method = PngInterlaceMethod::None;
		prefix.ihdr.crc = swap_bytes(Crc32{}.process(&prefix.ihdr.type, sizeof prefix.ihdr.type).process(&prefix.ihdr.data, sizeof prefix.ihdr.data).value());
		prefix.idat.length = 0;
		prefix.idat.type = PngChunkType::IDAT;

		const auto pixel_size = ImageInfo::pixel_size(pixel_format);
		const auto stride = 1 + info.width() * pixel_size;
		ImageInfo png_info{ info.width(), info.height(), stride, pixel_format, ImageOrientation::XRightYDown };

		Buffer image_buffer{ png_info.frame_size() };
		if (!Image::transform(info, data, png_info, image_buffer.begin() + 1))
			return false;

		for (std::size_t i = 0; i < image_buffer.size(); i += stride)
			image_buffer[i] = to_underlying(PngStandardFilterType::None);

		const auto compressed_buffer = make_zlib_buffer(image_buffer.data(), image_buffer.size());
		prefix.idat.length = swap_bytes(static_cast<std::uint32_t>(compressed_buffer.size()));

		PngSuffix suffix;
		suffix.idat.crc = swap_bytes(Crc32{}.process(&prefix.idat.type, sizeof prefix.idat.type).process(compressed_buffer.data(), compressed_buffer.size()).value());
		suffix.iend.length = 0;
		suffix.iend.type = PngChunkType::IEND;
		suffix.iend.crc = swap_bytes(Crc32{}.process(&suffix.iend.type, sizeof suffix.iend.type).value());

		writer.reserve(sizeof prefix + compressed_buffer.size() + sizeof suffix);
		return writer.write(prefix) && writer.write(compressed_buffer.data(), compressed_buffer.size()) && writer.write(suffix);
	}
}
