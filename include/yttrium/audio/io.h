/// \file
/// \brief Audio input/output facilities.

#ifndef __Y_AUDIO_IO_H
#define __Y_AUDIO_IO_H

#include <yttrium/pointer.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

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
	uint_fast8_t  bytes_per_sample; ///< Bytes per sample, usually 1 or 2.
	uint_fast8_t  channels;         ///< Sampling channels, usually 1 or 2.
	uint_fast32_t frequency;        ///< Sampling rate.

	///
	AudioFormat() noexcept
		: bytes_per_sample(0)
		, channels(0)
		, frequency(0)
	{
	}

	///
	uint_fast16_t unit_size() const noexcept
	{
		return bytes_per_sample * channels;
	}
};

class AudioReader;

/// AudioReader pointer.
typedef Pointer<AudioReader> AudioReaderPtr;

/// Read-only audio file access interface.
class Y_API AudioReader: public Pointable
{
public:

	~AudioReader() = default;

	/// Return the audio format.
	/// \return %Audio format.
	AudioFormat format() const noexcept { return _format; }

	/// Return the current audio offset.
	/// \return Current offset in samples.
	UOffset offset() noexcept { return _offset_units; }

	/// Read at most \a size bytes into \a buffer.
	/// \param buffer Buffer to read into.
	/// \param bytes_to_read Number of bytes to read.
	/// \return Number of bytes read or 0 on failure.
	/// \note The passed \a size is rounded down to the sample scale before the actual reading.
	virtual size_t read(void* buffer, size_t bytes_to_read) noexcept = 0;

	/// Move the audio offset to the specified position.
	/// \param offset %Offset in samples.
	/// \return \c true on success.
	virtual bool seek(UOffset offset) noexcept = 0;

	/// Return the opened audio size.
	/// \return File size in bytes.
	UOffset size() const noexcept { return _total_units * _format.unit_size(); }

public:

	///
	static AudioReaderPtr open(const StaticString& name, AudioType type = AudioType::Auto, Allocator* allocator = DefaultAllocator) noexcept;

protected:

	AudioFormat _format;
	UOffset     _total_units;
	UOffset     _offset_units;

	AudioReader(Allocator* allocator) noexcept
		: Pointable(allocator)
		, _total_units(0)
		, _offset_units(0)
	{
	}
};

} // namespace Yttrium

#endif // __Y_AUDIO_IO_H
