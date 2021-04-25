// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/endian.hpp>

namespace Yt
{
	enum class BmpFileType : uint16_t
	{
		Bm = primal::makeCC('B', 'M'),
	};

	enum class BmpCompression : uint32_t
	{
		Rgb = 0,
	};

#pragma pack(push, 1)

	struct BmpFileHeader
	{
		BmpFileType file_type;
		uint32_t file_size;
		uint32_t reserved;
		uint32_t data_offset;
	};

	struct BmpInfoHeader
	{
		uint32_t header_size;
		int32_t width;
		int32_t height;
		uint16_t planes;
		uint16_t bits_per_pixel;
		BmpCompression compression;
		uint32_t image_size;
		int32_t x_pixels_per_meter;
		int32_t y_pixels_per_meter;
		uint32_t used_colors;
		uint32_t required_colors;
	};

#pragma pack(pop)
}
