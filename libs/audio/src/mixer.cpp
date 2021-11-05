// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "mixer.h"

#include <yttrium/audio/reader.h>
#include <yttrium/audio/utils.h>
#include "sound.h"

#include <primal/dsp.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>

namespace Yt
{
	AudioMixer::AudioMixer(const AudioFormat& format)
		: _format{ format }
	{
		assert(_format.sample_type() == AudioSample::f32);
		assert(_format.channels() == 2);
	}

	void AudioMixer::playMusic(const std::shared_ptr<AudioReader>& music)
	{
		std::lock_guard lock{ _mutex };
		if (music == _music)
			return;
		if (music)
			music->restart();
		_music = music;
	}

	void AudioMixer::playSound(const std::shared_ptr<Sound>& sound)
	{
		std::lock_guard lock{ _mutex };
		_sound = std::static_pointer_cast<SoundImpl>(sound);
		if (_sound)
			_sound->_reader.restart();
	}

	size_t AudioMixer::onRead(float* buffer, size_t maxFrames) noexcept
	{
		std::memset(buffer, 0, maxFrames * _format.bytes_per_frame());
		std::lock_guard lock{ _mutex };
		if (_music && !read(buffer, maxFrames, _buffer, *_music))
			_music.reset();
		if (_sound && !read(buffer, maxFrames, _buffer, _sound->_reader))
			_sound.reset();
		return maxFrames;
	}

	bool AudioMixer::read(float* out, size_t outFrames, primal::Buffer<std::byte>& inBuffer, AudioReader& reader)
	{
		const auto inFormat = reader.format();
		inBuffer.reserve(outFrames * inFormat.bytes_per_frame());
		const auto inFrames = reader.read_frames(inBuffer.data(), outFrames);
		if (!inFrames)
			return false;
		switch (inFormat.sample_type())
		{
		case Yt::AudioSample::i16:
			switch (inFormat.channels())
			{
			case 1:
				primal::addSamples2x1D(out, reinterpret_cast<const int16_t*>(inBuffer.data()), inFrames);
				break;
			case 2:
				primal::addSamples1D(out, reinterpret_cast<const int16_t*>(inBuffer.data()), inFrames * 2);
				break;
			default:
				return false;
			}
			break;
		case Yt::AudioSample::f32:
			switch (inFormat.channels())
			{
			case 1:
				primal::addSamples2x1D(out, reinterpret_cast<const float*>(inBuffer.data()), inFrames);
				break;
			case 2:
				primal::addSamples1D(out, reinterpret_cast<const float*>(inBuffer.data()), inFrames * 2);
				break;
			default:
				return false;
			}
			break;
		}
		return true;
	}
}
