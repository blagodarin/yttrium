//
// Copyright 2019 Sergei Blagodarin
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

#include "backend.h"

#include <yttrium/exceptions.h>
#include "player.h"
#include "sound.h"

#ifndef NDEBUG
#	include <iostream>
#endif

namespace
{
	constexpr unsigned FramesPerSecond = 44'100;
	constexpr snd_pcm_uframes_t FramesPerPeriod = FramesPerSecond / 25;
	constexpr unsigned PeriodsPerBuffer = 2;

	constexpr std::string_view function_name(const char* signature) noexcept
	{
		auto end = signature;
		while (*end != '(')
			++end;
		return { signature, static_cast<size_t>(end - signature) };
	}
}

#define CHECK_ALSA(call) \
	do \
	{ \
		if (const auto x = call; x < 0) \
			throw BadCall{ "ALSA", ::function_name(#call), snd_strerror(x) }; \
	} while (false)

namespace Yttrium
{
	AlsaAudioBackend::AlsaAudioBackend()
		: _buffer_frames{ FramesPerPeriod * PeriodsPerBuffer }
		, _period_frames{ FramesPerPeriod }
	{
		snd_pcm_t* pcm_handle = nullptr;
		CHECK_ALSA(snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0));
		_pcm_handle.reset(pcm_handle);
		{
			snd_pcm_hw_params_t* pcm_hw_params = nullptr;
			CHECK_ALSA(snd_pcm_hw_params_malloc(&pcm_hw_params));
			const UniquePtr<snd_pcm_hw_params_t, snd_pcm_hw_params_free> pcm_hw_params_deleter{ pcm_hw_params };
			CHECK_ALSA(snd_pcm_hw_params_any(pcm_handle, pcm_hw_params));
			CHECK_ALSA(snd_pcm_hw_params_set_access(pcm_handle, pcm_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED));
			CHECK_ALSA(snd_pcm_hw_params_set_format(pcm_handle, pcm_hw_params, SND_PCM_FORMAT_S16_LE));
			CHECK_ALSA(snd_pcm_hw_params_set_channels(pcm_handle, pcm_hw_params, 2));
			CHECK_ALSA(snd_pcm_hw_params_set_rate(pcm_handle, pcm_hw_params, FramesPerSecond, 0));
			CHECK_ALSA(snd_pcm_hw_params_set_period_size_near(pcm_handle, pcm_hw_params, &_period_frames, nullptr));
			unsigned periods = PeriodsPerBuffer;
			CHECK_ALSA(snd_pcm_hw_params_set_periods_near(pcm_handle, pcm_hw_params, &periods, nullptr));
			CHECK_ALSA(snd_pcm_hw_params(pcm_handle, pcm_hw_params));
			CHECK_ALSA(snd_pcm_hw_params_get_period_size(pcm_hw_params, &_period_frames, nullptr));
			CHECK_ALSA(snd_pcm_hw_params_get_buffer_size(pcm_hw_params, &_buffer_frames));
#ifndef NDEBUG
			std::cerr << "[ALSA] PCM buffer: "
					  << _buffer_frames << " frames, "
					  << _buffer_frames * 4 << " bytes, "
					  << FramesPerSecond / static_cast<double>(_buffer_frames) << " buffers/s, "
					  << static_cast<double>(_buffer_frames * 1000) / FramesPerSecond << " ms/buffer\n";
			std::cerr << "[ALSA] PCM period: "
					  << _period_frames << " frames, "
					  << _period_frames * 4 << " bytes, "
					  << FramesPerSecond / static_cast<double>(_period_frames) << " periods/s, "
					  << static_cast<double>(_period_frames * 1000) / FramesPerSecond << " ms/period\n";
#endif
		}
	}

	AlsaAudioBackend::~AlsaAudioBackend() = default;

	std::unique_ptr<AudioPlayerBackend> AlsaAudioBackend::create_player()
	{
		return std::make_unique<AlsaAudioPlayer>();
	}

	std::unique_ptr<Sound> AlsaAudioBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<AlsaSound>(reader);
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<AlsaAudioBackend>();
	}
}
