// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/audio/sound.h>

#include <yttrium/audio/reader.h>

namespace Yt
{
	class SoundImpl final : public Sound
	{
	public:
		AudioReader _reader;

		explicit SoundImpl(std::unique_ptr<Source>&& source)
			: _reader{ std::move(source), false } {}

		~SoundImpl() override = default;
	};
}
