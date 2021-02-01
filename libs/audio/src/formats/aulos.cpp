//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2020 Sergei Blagodarin.
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

#include "aulos.h"

#include <yttrium/exceptions.h>

#include <aulos/composition.hpp>
#include <aulos/format.hpp>
#include <aulos/renderer.hpp>

#include <limits>

namespace Yt
{
	AulosDecoder::AulosDecoder(std::unique_ptr<Source>&& source)
		: AudioDecoder{ std::move(source) }
	{
		auto composition = aulos::Composition::create(_source->to_string().c_str());
		if (!composition)
			throw DataError("Bad Aulos file");
		const aulos::AudioFormat format{ 44'100, aulos::ChannelLayout::Stereo };
		_renderer = aulos::Renderer::create(*composition, format, true);
		_format = { AudioSample::f32, format.channelCount(), format.samplingRate() };
		_total_frames = std::numeric_limits<uint64_t>::max();
	}

	AulosDecoder::~AulosDecoder() noexcept = default;

	size_t AulosDecoder::read_frames(void* buffer, size_t frames)
	{
		const auto result = _renderer->render(static_cast<float*>(buffer), frames);
		_current_frame += result;
		return result;
	}

	bool AulosDecoder::seek_frame(uint64_t frame)
	{
		_renderer->restart();
		_current_frame = _renderer->skipFrames(static_cast<size_t>(frame));
		return true;
	}
}
