/// \file
/// \brief

#ifndef _include_yttrium_audio_reader_h_
#define _include_yttrium_audio_reader_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class AudioFormat;
	class Source;

	/// Read-only audio file access interface.
	class Y_API AudioReader
	{
	public:
		///
		static std::unique_ptr<AudioReader> open(std::unique_ptr<Source>&&);

		virtual ~AudioReader() noexcept = default;

		/// Returns the current sample offset.
		virtual uint64_t current_sample() const noexcept = 0;

		/// Returns the audio format.
		virtual AudioFormat format() const noexcept = 0;

		/// Read at most \a size bytes into \a buffer.
		/// \param buffer Buffer to read into.
		/// \param bytes_to_read Number of bytes to read.
		/// \return Number of bytes read or 0 on failure.
		/// \note The passed \a size is rounded down to the sample scale before the actual reading.
		virtual size_t read(void* buffer, size_t bytes_to_read) = 0;

		/// Moves the audio offset to the specified position in samples.
		virtual bool seek(uint64_t offset) = 0;

		/// Returns the audio size in bytes.
		virtual uint64_t total_bytes() const noexcept = 0;

		/// Returns the audio size in samples.
		virtual uint64_t total_samples() const noexcept = 0;
	};
}

#endif
