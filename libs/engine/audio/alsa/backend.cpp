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

#include <numeric>

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

	class AlsaError : public Yttrium::BadCall
	{
	public:
		AlsaError(std::string_view function, int error)
			: BadCall{ "ALSA", function, snd_strerror(error) } {}
	};
}

#define CHECK_ALSA(call) \
	do \
	{ \
		if (const auto x = call; x < 0) \
			throw AlsaError{ ::function_name(#call), x }; \
	} while (false)

namespace Yttrium
{
	AlsaAudioBackend::AlsaAudioBackend(unsigned frames_per_second)
		: _buffer_format{ AudioSample::f32, AudioBufferChannels, frames_per_second }
		, _block_frames{ std::lcm(BlockAlignment, _buffer_format.bytes_per_frame()) / _buffer_format.bytes_per_frame() }
	{
		CHECK_ALSA(snd_pcm_open(&_pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));
		{
			SlimPtr<snd_pcm_hw_params_t, snd_pcm_hw_params_free> hw;
			CHECK_ALSA(snd_pcm_hw_params_malloc(&hw));
			CHECK_ALSA(snd_pcm_hw_params_any(_pcm, hw));
			CHECK_ALSA(snd_pcm_hw_params_set_access(_pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED));
			CHECK_ALSA(snd_pcm_hw_params_set_format(_pcm, hw, SND_PCM_FORMAT_FLOAT));
			CHECK_ALSA(snd_pcm_hw_params_set_channels(_pcm, hw, AudioBufferChannels));
			CHECK_ALSA(snd_pcm_hw_params_set_rate(_pcm, hw, frames_per_second, 0));
			unsigned periods = PeriodsPerBuffer;
			CHECK_ALSA(snd_pcm_hw_params_set_periods_near(_pcm, hw, &periods, nullptr));
			snd_pcm_uframes_t min_period = 0;
			int dir = 0;
			CHECK_ALSA(snd_pcm_hw_params_get_period_size_min(hw, &min_period, &dir));
			_period_frames = (min_period + _block_frames - 1) / _block_frames * _block_frames;
			CHECK_ALSA(snd_pcm_hw_params_set_period_size(_pcm, hw, _period_frames, _period_frames == min_period ? dir : 0));
			CHECK_ALSA(snd_pcm_hw_params(_pcm, hw));
			CHECK_ALSA(snd_pcm_hw_params_get_period_size(hw, &_period_frames, nullptr));
			CHECK_ALSA(snd_pcm_hw_params_get_buffer_size(hw, &_buffer_frames));
		}
		{
			SlimPtr<snd_pcm_sw_params_t, snd_pcm_sw_params_free> sw;
			CHECK_ALSA(snd_pcm_sw_params_malloc(&sw));
			CHECK_ALSA(snd_pcm_sw_params_current(_pcm, sw));
			CHECK_ALSA(snd_pcm_sw_params_set_avail_min(_pcm, sw, _period_frames));
			CHECK_ALSA(snd_pcm_sw_params_set_start_threshold(_pcm, sw, 1));
			CHECK_ALSA(snd_pcm_sw_params_set_stop_threshold(_pcm, sw, _buffer_frames));
			CHECK_ALSA(snd_pcm_sw_params(_pcm, sw));
		}
		_period.reset(_period_frames * _buffer_format.bytes_per_frame());
	}

	AlsaAudioBackend::~AlsaAudioBackend() = default;

	void AlsaAudioBackend::play_buffer()
	{
		auto data = _period.begin();
		auto frames_left = _period_frames;
		while (frames_left > 0)
		{
			const auto result = snd_pcm_writei(_pcm, data, frames_left);
			if (result < 0)
			{
				if (result != -EAGAIN)
					if (const auto recovered = snd_pcm_recover(_pcm, static_cast<int>(result), 1); recovered < 0)
						throw AlsaError{ "snd_pcm_writei", recovered };
				continue;
			}
			if (result == 0)
			{
				snd_pcm_wait(_pcm, static_cast<int>((_buffer_frames * 1000 + _buffer_format.frames_per_second() - 1) / _buffer_format.frames_per_second()));
				continue;
			}
			data += to_unsigned(result) * _buffer_format.bytes_per_frame();
			frames_left -= to_unsigned(result);
		}
	}

	void AlsaAudioBackend::begin_context()
	{
	}

	void AlsaAudioBackend::end_context() noexcept
	{
		snd_pcm_drain(_pcm);
	}

	AudioBackend::BufferView AlsaAudioBackend::lock_buffer()
	{
		return { _period.data(), _period_frames };
	}

	void AlsaAudioBackend::unlock_buffer(bool) noexcept
	{
	}

	std::unique_ptr<AudioBackend> AudioBackend::create(unsigned frames_per_second)
	{
		return std::make_unique<AlsaAudioBackend>(frames_per_second);
	}
}
