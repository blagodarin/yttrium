// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace Yt
{
	class AudioFormat;
	class Writer;

	///
	bool transform_audio(void* dst, const AudioFormat& dst_format, const void* src, const AudioFormat& src_format, size_t frames);

	///
	bool write_wav_header(Writer&, const AudioFormat&, size_t samples);
}
