// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../decoder.h"

namespace aulos
{
	class Renderer;
}

namespace Yt
{
	class AulosDecoder final : public AudioDecoder
	{
	public:
		explicit AulosDecoder(std::unique_ptr<Source>&&, bool looping);
		~AulosDecoder() noexcept override;

		size_t read_frames(void*, size_t) override;
		bool seek_frame(uint64_t) override;

	private:
		std::unique_ptr<aulos::Renderer> _renderer;
	};
}
