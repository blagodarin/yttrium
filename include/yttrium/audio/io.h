/// \file
/// \brief Audio input/output facilities.

#ifndef _include_yttrium_audio_io_h_
#define _include_yttrium_audio_io_h_

#include <yttrium/global.h>

#include <cstddef>
#include <cstdint>

namespace Yttrium
{
	class Allocator;
	class StaticString;

	template <typename> class UniquePtr;

	/// Audio container-codec combo.
	enum class AudioType
	{
		Auto,      ///< Automatical detection.
		Wav,       ///< PCM waveform audio (WAV) file.
		OggVorbis, ///< Ogg/Vorbis file.
	};

	/// Audio data format.
	struct AudioFormat
	{
		uint_fast8_t  bytes_per_sample = 0; ///< Bytes per sample, usually 1 or 2.
		uint_fast8_t  channels = 0;         ///< Sampling channels, usually 1 or 2.
		uint_fast32_t frequency = 0;        ///< Sampling rate.

		///
		uint_fast16_t unit_size() const { return bytes_per_sample * channels; }
	};

	/// Read-only audio file access interface.
	class Y_API AudioReader
	{
	public:

		///
		static UniquePtr<AudioReader> open(const StaticString& name, AudioType type = AudioType::Auto, Allocator* allocator = nullptr);

		virtual ~AudioReader() = default;

		/// Returns the audio format.
		virtual AudioFormat format() const = 0;

		/// Return the current audio offset in samples.
		virtual uint64_t offset() const = 0;

		/// Read at most \a size bytes into \a buffer.
		/// \param buffer Buffer to read into.
		/// \param bytes_to_read Number of bytes to read.
		/// \return Number of bytes read or 0 on failure.
		/// \note The passed \a size is rounded down to the sample scale before the actual reading.
		virtual size_t read(void* buffer, size_t bytes_to_read) = 0;

		/// Moves the audio offset to the specified position in samples.
		virtual bool seek(uint64_t offset) = 0;

		/// Returns the audio size in bytes.
		virtual uint64_t size() const = 0;
	};
}

#endif
