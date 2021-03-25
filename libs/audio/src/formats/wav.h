// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../decoder.h"

namespace Yt
{
	class WavDecoder final : public AudioDecoder
	{
	public:
		explicit WavDecoder(std::unique_ptr<Source>&&);

		size_t read_frames(void*, size_t) override;
		bool seek_frame(uint64_t) override;

	private:
		uint64_t _data_offset = 0;
	};
}
