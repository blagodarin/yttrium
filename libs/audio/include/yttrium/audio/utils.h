//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <yttrium/audio/api.h>

#include <cstddef>

namespace Yt
{
	class AudioFormat;
	class Writer;

	///
	Y_AUDIO_API bool transform_audio(void* dst, const AudioFormat& dst_format, const void* src, const AudioFormat& src_format, size_t frames);

	///
	Y_AUDIO_API bool write_wav_header(Writer&, const AudioFormat&, size_t samples);
}
