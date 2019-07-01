//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include <yttrium/utils/numeric.h>

namespace Yttrium
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
			RIFF = make_cc('R', 'I', 'F', 'F'),
			WAVE = make_cc('W', 'A', 'V', 'E'),
		};

		uint32_t riff_fourcc;
		uint32_t riff_size;
		uint32_t wave_fourcc;
	};

	struct WavChunkHeader
	{
		enum : uint32_t
		{
			fmt = make_cc('f', 'm', 't', ' '),
			data = make_cc('d', 'a', 't', 'a'),
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
