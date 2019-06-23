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

namespace
{
	constexpr unsigned AudioBufferChannels = 2;
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
	AlsaAudioBackend::AlsaAudioBackend(unsigned frames_per_second)
	{
		snd_pcm_t* pcm = nullptr;
		CHECK_ALSA(snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));
		_pcm.reset(pcm);
		auto period_frames = snd_pcm_uframes_t{ frames_per_second } / 25;
		auto buffer_frames = period_frames * PeriodsPerBuffer;
		{
			snd_pcm_hw_params_t* hw = nullptr;
			CHECK_ALSA(snd_pcm_hw_params_malloc(&hw));
			const UniquePtr<snd_pcm_hw_params_t, snd_pcm_hw_params_free> hw_deleter{ hw };
			CHECK_ALSA(snd_pcm_hw_params_any(pcm, hw));
			CHECK_ALSA(snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED));
			CHECK_ALSA(snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_FLOAT));
			CHECK_ALSA(snd_pcm_hw_params_set_channels(pcm, hw, AudioBufferChannels));
			CHECK_ALSA(snd_pcm_hw_params_set_rate(pcm, hw, frames_per_second, 0));
			CHECK_ALSA(snd_pcm_hw_params_set_period_size_near(pcm, hw, &period_frames, nullptr));
			unsigned periods = PeriodsPerBuffer;
			CHECK_ALSA(snd_pcm_hw_params_set_periods_near(pcm, hw, &periods, nullptr));
			CHECK_ALSA(snd_pcm_hw_params(pcm, hw));
			CHECK_ALSA(snd_pcm_hw_params_get_period_size(hw, &period_frames, nullptr));
			CHECK_ALSA(snd_pcm_hw_params_get_buffer_size(hw, &buffer_frames));
		}
		{
			snd_pcm_sw_params_t* sw = nullptr;
			CHECK_ALSA(snd_pcm_sw_params_malloc(&sw));
			const UniquePtr<snd_pcm_sw_params_t, snd_pcm_sw_params_free> sw_deleter{ sw };
			CHECK_ALSA(snd_pcm_sw_params_current(pcm, sw));
			CHECK_ALSA(snd_pcm_sw_params_set_avail_min(pcm, sw, period_frames));
			CHECK_ALSA(snd_pcm_sw_params_set_start_threshold(pcm, sw, 1));
			CHECK_ALSA(snd_pcm_sw_params_set_stop_threshold(pcm, sw, buffer_frames));
			CHECK_ALSA(snd_pcm_sw_params(pcm, sw));
		}
		_buffer_info._format = { AudioSample::f32, AudioBufferChannels, frames_per_second };
		_buffer_info._size = period_frames * _buffer_info._format.bytes_per_frame();
		_buffer.reset(_buffer_info._size);
	}

	AlsaAudioBackend::~AlsaAudioBackend() = default;

	void AlsaAudioBackend::play_buffer()
	{
		auto data = _buffer.begin();
		const auto buffer_frames = _buffer.size() / _buffer_info._format.bytes_per_frame();
		auto frames_left = buffer_frames;
		while (frames_left > 0)
		{
			const auto result = snd_pcm_writei(_pcm.get(), data, frames_left);
			if (result < 0)
			{
				if (result != -EAGAIN)
					if (const auto recovered = snd_pcm_recover(_pcm.get(), static_cast<int>(result), 1); recovered < 0)
						throw BadCall{ "ALSA", "snd_pcm_writei", snd_strerror(recovered) };
				continue;
			}
			if (result == 0)
			{
				snd_pcm_wait(_pcm.get(), static_cast<int>((buffer_frames * 1000 + _buffer_info._format.frames_per_second() - 1) / _buffer_info._format.frames_per_second()));
				continue;
			}
			data += to_unsigned(result) * _buffer_info._format.bytes_per_frame();
			frames_left -= to_unsigned(result);
		}
	}

	void AlsaAudioBackend::begin_context()
	{
	}

	void AlsaAudioBackend::end_context() noexcept
	{
		snd_pcm_drain(_pcm.get());
	}

	void* AlsaAudioBackend::lock_buffer()
	{
		return _buffer.data();
	}

	void AlsaAudioBackend::unlock_buffer(bool) noexcept
	{
	}

	std::unique_ptr<AudioBackend> AudioBackend::create(unsigned frames_per_second)
	{
		return std::make_unique<AlsaAudioBackend>(frames_per_second);
	}
}
