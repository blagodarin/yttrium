// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <primal/endian.hpp>

namespace Yt
{
	enum : uint16_t
	{
		WAVE_FORMAT_PCM = 0x0001,
		WAVE_FORMAT_IEEE_FLOAT = 0x0003,
	};

#pragma pack(push, 1)

	struct WavFileHeader
	{
		enum : uint32_t
		{
			RIFF = primal::makeCC('R', 'I', 'F', 'F'),
			WAVE = primal::makeCC('W', 'A', 'V', 'E'),
		};

		uint32_t riff_fourcc;
		uint32_t riff_size;
		uint32_t wave_fourcc;
	};

	struct WavChunkHeader
	{
		enum : uint32_t
		{
			fmt = primal::makeCC('f', 'm', 't', ' '),
			data = primal::makeCC('d', 'a', 't', 'a'),
		};

		uint32_t name_fourcc;
		uint32_t size;
	};

	struct WavFormatChunk
	{
		uint16_t format;
		uint16_t channels;
		uint32_t samples_per_second;
		uint32_t bytes_per_second;
		uint16_t block_align;
		uint16_t bits_per_sample;
	};

#pragma pack(pop)
}
