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

#ifndef _include_yttrium_audio_music_reader_h_
#define _include_yttrium_audio_music_reader_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class Source;

	/// Music data with corresponding audio decoder.
	class Y_ENGINE_API MusicReader
	{
	public:
		///
		/// Returns 'nullptr' for a null Source pointer.
		/// May throw DataError.
		static std::unique_ptr<MusicReader> open(std::unique_ptr<Source>&&);

		virtual ~MusicReader() noexcept = default;

		///
		/// \param end_ms Offset to the end of a playback range.
		/// \param loop_ms Offset to the start of a playback loop.
		virtual bool set_properties(int end_ms, int loop_ms) noexcept = 0;
	};
}

#endif
