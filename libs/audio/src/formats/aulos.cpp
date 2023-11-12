// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "aulos.h"

#include <yttrium/base/exceptions.h>

#include <seir_synth/composition.hpp>
#include <seir_synth/format.hpp>
#include <seir_synth/renderer.hpp>

#include <limits>

namespace Yt
{
	AulosDecoder::AulosDecoder(std::unique_ptr<Source>&& source, bool looping)
		: AudioDecoder{ std::move(source) }
	{
		auto composition = seir::synth::Composition::create(_source->to_string().c_str());
		if (!composition)
			throw DataError("Bad Seir Synth file");
		const seir::synth::AudioFormat format{ 44'100, seir::synth::ChannelLayout::Stereo };
		_renderer = seir::synth::Renderer::create(*composition, format, looping);
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
