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

#pragma once

#if !Y_USE_OGG_VORBIS
#	error Ogg Vorbis support is disabled.
#endif

#include "../decoder.h"

#include <vorbis/vorbisfile.h>

namespace Yttrium
{
	class OggVorbisDecoder final : public AudioDecoder
	{
	public:
		explicit OggVorbisDecoder(std::unique_ptr<Source>&&);
		~OggVorbisDecoder() override;

		size_t read_frames(void*, size_t) override;
		bool seek_frame(uint64_t) override;

	private:
		OggVorbis_File _ov_file;
	};
}
