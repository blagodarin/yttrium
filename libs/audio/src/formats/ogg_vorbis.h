// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../decoder.h"

#include <vorbis/vorbisfile.h>

namespace Yt
{
	class OggVorbisDecoder final : public AudioDecoder
	{
	public:
		explicit OggVorbisDecoder(std::unique_ptr<Source>&&);
		~OggVorbisDecoder() override;

		size_t read_frames(void*, size_t) override;
		bool seek_frame(uint64_t) override;

	private:
		OggVorbis_File _ov_file;
	};
}
