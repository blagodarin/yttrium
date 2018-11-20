//
// Copyright 2018 Sergei Blagodarin
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

#ifndef _src_audio_backend_h_
#define _src_audio_backend_h_

#include <memory>

namespace Yttrium
{
	class AudioFormat;
	class AudioReader;
	class Sound;

	class AudioPlayerBackend
	{
	public:
		enum
		{
			NumBuffers = 2,
		};

		virtual ~AudioPlayerBackend() = default;

		virtual void set_format(const AudioFormat&) = 0;
		virtual void fill_buffer(size_t index, void* data, size_t size) = 0;
		virtual size_t check_buffers() = 0;
		virtual void refill_buffer(void* data, size_t size) = 0;
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual void stop() noexcept = 0;
	};

	class AudioBackend
	{
	public:
		static std::unique_ptr<AudioBackend> create();

		virtual ~AudioBackend() = default;

		virtual std::unique_ptr<AudioPlayerBackend> create_player() = 0;
		virtual std::unique_ptr<Sound> create_sound(AudioReader&) = 0;
	};
}

#endif
