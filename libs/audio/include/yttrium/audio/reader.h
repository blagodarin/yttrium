// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <memory>

namespace Yt
{
	class AudioDecoder;
	class AudioFormat;
	class Source;

	/// Read-only audio file access interface.
	class AudioReader
	{
	public:
		///
		explicit AudioReader(std::unique_ptr<Source>&&);
		~AudioReader() noexcept;

		/// Returns audio format.
		AudioFormat format() const noexcept;

		///
		size_t read_frames(void* buffer, size_t frames);

		///
		void restart();

		///
		bool set_loop(std::chrono::milliseconds from, std::chrono::milliseconds to = std::chrono::milliseconds::max()) noexcept;

		AudioReader(const AudioReader&) = delete;
		AudioReader& operator=(const AudioReader&) = delete;

	private:
		const std::unique_ptr<AudioDecoder> _decoder;
		uint64_t _end_frame;
		uint64_t _loop_frame;
	};
}
