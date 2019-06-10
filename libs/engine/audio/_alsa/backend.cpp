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
#include <yttrium/utils/numeric.h>
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
		snd_pcm_t* pcm = nullptr;
		CHECK_ALSA(snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));
		_pcm.reset(pcm);
		{
			snd_pcm_hw_params_t* hw = nullptr;
			CHECK_ALSA(snd_pcm_hw_params_malloc(&hw));
			const UniquePtr<snd_pcm_hw_params_t, snd_pcm_hw_params_free> hw_deleter{ hw };
			CHECK_ALSA(snd_pcm_hw_params_any(pcm, hw));
			CHECK_ALSA(snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED));
			CHECK_ALSA(snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE));
			CHECK_ALSA(snd_pcm_hw_params_set_channels(pcm, hw, 2));
			CHECK_ALSA(snd_pcm_hw_params_set_rate(pcm, hw, FramesPerSecond, 0));
			CHECK_ALSA(snd_pcm_hw_params_set_period_size_near(pcm, hw, &_period_frames, nullptr));
			unsigned periods = PeriodsPerBuffer;
			CHECK_ALSA(snd_pcm_hw_params_set_periods_near(pcm, hw, &periods, nullptr));
			CHECK_ALSA(snd_pcm_hw_params(pcm, hw));
			CHECK_ALSA(snd_pcm_hw_params_get_period_size(hw, &_period_frames, nullptr));
			CHECK_ALSA(snd_pcm_hw_params_get_buffer_size(hw, &_buffer_frames));
		}
		_period_bytes = static_cast<size_t>(snd_pcm_frames_to_bytes(pcm, static_cast<snd_pcm_sframes_t>(_period_frames)));
#ifndef NDEBUG
		const auto buffer_bytes = snd_pcm_frames_to_bytes(pcm, static_cast<snd_pcm_sframes_t>(_buffer_frames));
		const auto buffers_per_second = FramesPerSecond / static_cast<double>(_buffer_frames);
		std::cerr << "[ALSA] PCM buffer: "
				  << _buffer_frames << " frames (" << buffer_bytes << " bytes), "
				  << buffers_per_second << " buffers/s (" << buffers_per_second * static_cast<double>(buffer_bytes) << " bytes/s), "
				  << static_cast<double>(_buffer_frames * 1000) / FramesPerSecond << " ms/buffer\n";
		const auto periods_per_second = FramesPerSecond / static_cast<double>(_period_frames);
		std::cerr << "[ALSA] PCM period: "
				  << _period_frames << " frames (" << _period_bytes << " bytes), "
				  << periods_per_second << " periods/s (" << periods_per_second * static_cast<double>(_period_bytes) << " bytes/s), "
				  << static_cast<double>(_period_frames * 1000) / FramesPerSecond << " ms/period\n";
#endif
		{
			snd_pcm_sw_params_t* sw = nullptr;
			CHECK_ALSA(snd_pcm_sw_params_malloc(&sw));
			const UniquePtr<snd_pcm_sw_params_t, snd_pcm_sw_params_free> sw_deleter{ sw };
			CHECK_ALSA(snd_pcm_sw_params_current(pcm, sw));
			CHECK_ALSA(snd_pcm_sw_params_set_avail_min(pcm, sw, _period_frames));
			CHECK_ALSA(snd_pcm_sw_params_set_start_threshold(pcm, sw, 1));
			CHECK_ALSA(snd_pcm_sw_params_set_stop_threshold(pcm, sw, _buffer_frames));
			CHECK_ALSA(snd_pcm_sw_params(pcm, sw));
		}
	}

	AlsaAudioBackend::~AlsaAudioBackend() = default;

	AudioBackend::BufferInfo AlsaAudioBackend::buffer_info() const noexcept
	{
		return { { 2, 2, FramesPerSecond }, _period_bytes };
	}

	std::unique_ptr<AudioPlayerBackend> AlsaAudioBackend::create_player()
	{
		return std::make_unique<AlsaAudioPlayer>();
	}

	std::unique_ptr<Sound> AlsaAudioBackend::create_sound(AudioReader& reader)
	{
		return std::make_unique<AlsaSound>(reader);
	}

	void AlsaAudioBackend::flush() noexcept
	{
		snd_pcm_drain(_pcm.get());
	}

	bool AlsaAudioBackend::write_buffer(const uint8_t* data, const std::atomic<bool>& done) noexcept
	{
		const auto frame_bytes = _period_bytes / _period_frames;
		auto frames_left = _period_frames;
		while (frames_left > 0 && !done)
		{
			const auto result = snd_pcm_writei(_pcm.get(), data, frames_left);
			if (result < 0)
			{
				if (result != -EAGAIN)
				{
					if (const auto recovered = snd_pcm_recover(_pcm.get(), static_cast<int>(result), 1); recovered < 0)
					{
#ifndef NDEBUG
						std::cerr << "[ALSA] Unrecoverable error: (" << recovered << ") " << snd_strerror(recovered) << '\n';
#endif
						return false;
					}
				}
				continue;
			}
			if (result == 0)
			{
				snd_pcm_wait(_pcm.get(), static_cast<int>((_period_frames * 1000 + FramesPerSecond - 1) / FramesPerSecond));
				continue;
			}
			data += to_unsigned(result) * frame_bytes;
			frames_left -= to_unsigned(result);
		}
		return true;
	}

	std::unique_ptr<AudioBackend> AudioBackend::create()
	{
		return std::make_unique<AlsaAudioBackend>();
	}
}
