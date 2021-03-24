// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Yt
{
	enum class IcoFileType : uint16_t
	{
		Ico = 1,
		Cur = 2,
	};

#pragma pack(push, 1)

	struct IcoFileHeader
	{
		uint16_t reserved;
		IcoFileType type;
		uint16_t count;
	};

	struct IcoImageHeader
	{
		uint8_t width;
		uint8_t height;
		uint8_t color_count;
		uint8_t reserved;
		union
		{
			struct
			{
				uint16_t color_planes;
				uint16_t bits_per_pixel;
			} ico;
			struct
			{
				uint16_t hotspot_left;
				uint16_t hotspot_top;
			} cur;
		};
		uint32_t data_size;
		uint32_t data_offset;
	};

#pragma pack(pop)
}
