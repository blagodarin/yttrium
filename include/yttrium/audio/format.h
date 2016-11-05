/// \file
/// \brief

#ifndef _include_yttrium_audio_format_h_
#define _include_yttrium_audio_format_h_

namespace Yttrium
{
	/// Audio data format.
	class AudioFormat
	{
	public:
		///
		AudioFormat() = default;

		///
		AudioFormat(unsigned bytes_per_sample, unsigned channels, unsigned frequency)
			: _bytes_per_sample(bytes_per_sample), _channels(channels), _frequency(frequency) {}

		/// Returns the number of bytes per sample (usually 1 or 2).
		unsigned bytes_per_sample() const { return _bytes_per_sample; }

		/// Returns the number of channels (usually 1 or 2).
		unsigned channels() const { return _channels; }

		/// Returns the number of samples per second.
		unsigned frequency() const { return _frequency; }

		///
		unsigned unit_size() const { return _bytes_per_sample * _channels; }

	private:
		unsigned _bytes_per_sample = 0;
		unsigned _channels = 0;
		unsigned _frequency = 0;
	};
}

#endif
