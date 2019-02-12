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

#pragma once

#include "../../utils/fourcc.h"

namespace Yttrium
{
	class ImageInfo;

	constexpr auto PngSignature = "\x89PNG\r\n\x1a\n"_eightcc;

	enum class PngChunkType : std::uint32_t
	{
		IDAT = "IDAT"_fourcc,
		IEND = "IEND"_fourcc,
		IHDR = "IHDR"_fourcc,
	};

	enum class PngColorType : std::uint8_t
	{
		Grayscale = 0,
		Truecolor = 2,
		Indexed = 3,
		GrayscaleAlpha = 4,
		TruecolorAlpha = 6,
	};

	enum class PngCompressionMethod : std::uint8_t
	{
		Zlib = 0,
	};

	enum class PngFilterMethod : std::uint8_t
	{
		Standard = 0,
	};

	enum class PngStandardFilterType : std::uint8_t
	{
		None = 0,
		Sub = 1,
		Up = 2,
		Average = 3,
		Paeth = 4,
	};

	enum class PngInterlaceMethod : std::uint8_t
	{
		None = 0,
		Adam7 = 1,
	};

#pragma pack(push, 1)

	struct PngPrefix
	{
		std::uint64_t signature;
		struct
		{
			std::uint32_t length;
			PngChunkType type;
			struct
			{
				std::uint32_t width;
				std::uint32_t height;
				std::uint8_t bit_depth;
				PngColorType color_type;
				PngCompressionMethod compression_method;
				PngFilterMethod filter_method;
				PngInterlaceMethod interlace_method;
			} data;
			std::uint32_t crc;
		} ihdr;
		struct
		{
			std::uint32_t length;
			PngChunkType type;
		} idat;
	};

	struct PngSuffix
	{
		struct
		{
			std::uint32_t crc;
		} idat;
		struct
		{
			std::uint32_t length;
			PngChunkType type;
			std::uint32_t crc;
		} iend;
	};

#pragma pack(pop)
}
