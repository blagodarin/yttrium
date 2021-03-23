// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	class AudioReader;
	class Sound;
	class Source;

	/// Audio manager.
	class AudioManager
	{
	public:
		///
		static std::shared_ptr<AudioManager> create();

		virtual ~AudioManager() noexcept = default;

		///
		virtual std::shared_ptr<Sound> create_sound(std::unique_ptr<Source>&&) = 0;

		///
		virtual void play_music(const std::shared_ptr<AudioReader>&) = 0;

		///
		virtual void play_sound(const std::shared_ptr<Sound>&) = 0;
	};
}
