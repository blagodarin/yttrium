/// \file
/// \brief Audio input/output facilities.

#ifndef __Y_AUDIO_IO_HPP
#define __Y_AUDIO_IO_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/static_string.hpp>

namespace Yttrium
{

/// Audio container-codec combo.

enum class AudioType
{
	Auto,      ///< Automatical detection (read only).
	Wav,       ///< PCM waveform audio (WAV) file.
	OggVorbis, ///< Ogg/Vorbis file.
};

/// Audio data format.

struct AudioFormat
{
	uint_fast8_t  depth;     ///< Bytes per channel per sample, usually 1 or 2.
	uint_fast8_t  channels;  ///< Channels per sample, usually 1 or 2.
	uint_fast32_t frequency; ///< Samples per second rate.

	///

	AudioFormat() noexcept
		: depth(0)
		, channels(0)
		, frequency(0)
	{
	}

	///

	uint_fast16_t atom_size() const noexcept
	{
		return depth * channels;
	}
};

/// Read-only audio file access interface.

class AudioReader
{
public:

	///

	AudioReader() noexcept
		: _private(nullptr)
	{
	}

	///

	AudioReader(const StaticString &name, AudioType type = AudioType::Auto, Allocator *allocator = HeapAllocator::instance()) noexcept
		//: AudioReader() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(name, type, allocator);
	}

	///

	AudioReader(const AudioReader &reader) noexcept;

	///

	~AudioReader() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	/// Return the audio format.
	/// \return %Audio format.

	AudioFormat format() const noexcept;

	/// Return the current audio offset.
	/// \return Current offset in samples.

	UOffset offset() noexcept;

	///

	bool open(const StaticString &name, AudioType type = AudioType::Auto, Allocator *allocator = HeapAllocator::instance()) noexcept;

	///

	bool opened() const noexcept
	{
		return _private;
	}

	/// Read at most \a size bytes into \a buffer.
	/// \param buffer Buffer to read into.
	/// \param size Number of bytes to read.
	/// \return Number of bytes read or 0 on failure.
	/// \note The passed \a size is rounded down to the sample scale before the actual reading.

	size_t read(void *buffer, size_t size) noexcept;

	/// Move the audio offset to the specified position.
	/// \param offset %Offset in samples.

	bool seek(UOffset offset) noexcept;

	/// Return the opened audio size.
	/// \return File size in samples.

	UOffset size() const noexcept;

public:

	///

	operator SafeBool() const noexcept
	{
		return Y_SAFE_BOOL(_private);
	}

public:

	class Private;

private:

	Private *_private;
};

} // namespace Yttrium

#endif // __Y_AUDIO_IO_HPP
