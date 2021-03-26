// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "aulos.h"

#include <yttrium/base/exceptions.h>

#include <aulos/composition.hpp>
#include <aulos/format.hpp>
#include <aulos/renderer.hpp>

#include <limits>

namespace Yt
{
	AulosDecoder::AulosDecoder(std::unique_ptr<Source>&& source, bool looping)
		: AudioDecoder{ std::move(source) }
	{
		auto composition = aulos::Composition::create(_source->to_string().c_str());
		if (!composition)
			throw DataError("Bad Aulos file");
		const aulos::AudioFormat format{ 44'100, aulos::ChannelLayout::Stereo };
		_renderer = aulos::Renderer::create(*composition, format, looping);
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
