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

#ifndef _include_yttrium_audio_reader_h_
#define _include_yttrium_audio_reader_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class AudioFormat;
	class Source;

	/// Read-only audio file access interface.
	class Y_CORE_API AudioReader
	{
	public:
		///
		static std::unique_ptr<AudioReader> open(std::unique_ptr<Source>&&);

		virtual ~AudioReader() noexcept = default;

		/// Returns the audio format.
		virtual AudioFormat format() const noexcept = 0;

		/// Reads at most \a size bytes into \a buffer.
		/// \param buffer Buffer to read into.
		/// \param bytes_to_read Number of bytes to read.
		/// \return Number of bytes read or 0 on failure.
		/// \note The passed \a size is rounded down to the frame scale before the actual reading.
		virtual size_t read(void* buffer, size_t bytes_to_read) = 0;

		/// Moves the audio offset to the specified position in frames.
		virtual bool seek(uint64_t offset) = 0;

		/// Returns the audio size in bytes.
		virtual uint64_t total_bytes() const noexcept = 0;

		/// MusicReader interface.
		virtual uint64_t current_frame() const noexcept = 0;
		virtual uint64_t total_frames() const noexcept = 0;
	};
}

#endif
