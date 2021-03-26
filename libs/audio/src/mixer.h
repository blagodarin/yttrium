// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/audio/format.h>
#include <yttrium/base/buffer.h>

#include <aulosplay/player.hpp>

#include <mutex>

namespace Yt
{
	class AudioReader;
	class Sound;
	class SoundImpl;

	class AudioMixer : public aulosplay::Source
	{
	public:
		explicit AudioMixer(const AudioFormat&);

		void play_music(const std::shared_ptr<AudioReader>&);
		void play_sound(const std::shared_ptr<Sound>&);

	private:
		bool isStereo() const noexcept override { return true; }
		size_t onRead(float* buffer, size_t maxFrames) noexcept override;

	private:
		bool read(void* out, size_t out_frames, Buffer& in_buffer, AudioReader&);

	private:
		const AudioFormat _format;
		Buffer _mix_buffer;
		Buffer _conversion_buffer;
		std::mutex _mutex;
		std::shared_ptr<AudioReader> _music;
		std::shared_ptr<SoundImpl> _sound;
	};
}
