// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "mixer.h"

#include <yttrium/audio/reader.h>
#include <yttrium/audio/utils.h>
#include "sound.h"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace
{
	void addSaturateF32(void* dst, const void* src, size_t count) noexcept
	{
		for (size_t i = 0; i < count; ++i)
			static_cast<float*>(dst)[i] = std::clamp(static_cast<float*>(dst)[i] + static_cast<const float*>(src)[i], -1.f, 1.f);
	}
}

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
		std::lock_guard lock{ _mutex };
		if (!_music && !_sound)
		{
			std::memset(buffer, 0, maxFrames * _format.bytes_per_frame());
		}
		else
		{
			auto out = buffer;
			if (_music)
			{
				if (!read(out, maxFrames, _conversionBuffer, *_music))
					_music.reset();
				else
				{
					_mixBuffer.reserve(maxFrames * _format.bytes_per_frame());
					out = reinterpret_cast<float*>(_mixBuffer.data());
				}
			}
			if (_sound)
			{
				if (!read(out, maxFrames, _conversionBuffer, _sound->_reader))
					_sound.reset();
				else if (out != buffer)
					::addSaturateF32(buffer, out, maxFrames * _format.channels());
			}
		}
		return maxFrames;
	}

	bool AudioMixer::read(void* out, size_t outFrames, primal::Buffer<std::byte>& inBuffer, AudioReader& reader)
	{
		if (const auto inFormat = reader.format(); inFormat == _format)
		{
			const auto inFrames = reader.read_frames(out, outFrames);
			if (!inFrames)
				return false;
			if (inFrames < outFrames)
				std::memset(static_cast<std::byte*>(out) + inFrames * _format.bytes_per_frame(), 0, (outFrames - inFrames) * _format.bytes_per_frame());
		}
		else
		{
			inBuffer.reserve(outFrames * inFormat.bytes_per_frame());
			const auto inFrames = reader.read_frames(inBuffer.data(), outFrames);
			if (!inFrames)
				return false;
			if (!transform_audio(out, _format, inBuffer.data(), inFormat, inFrames))
				return false;
			if (inFrames < outFrames)
				std::memset(static_cast<std::byte*>(out) + inFrames * _format.bytes_per_frame(), 0, (outFrames - inFrames) * _format.bytes_per_frame());
		}
		return true;
	}
}
