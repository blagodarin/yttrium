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
		AudioFormat(size_t bytes_per_sample, size_t channels, size_t samples_per_second)
			: _bytes_per_sample(bytes_per_sample), _channels(channels), _samples_per_second(samples_per_second) {}

		///
		size_t block_size() const { return _bytes_per_sample * _channels; }

		/// Returns the number of bytes per sample (usually 1 or 2).
		size_t bytes_per_sample() const { return _bytes_per_sample; }

		/// Returns the number of channels (usually 1 or 2).
		size_t channels() const { return _channels; }

		/// Returns the number of bytes per second.
		size_t bytes_per_second() const { return _bytes_per_sample * _channels * _samples_per_second; }

		/// Returns the number of samples per second for each channel.
		size_t samples_per_second() const { return _samples_per_second; }

	private:
		size_t _bytes_per_sample = 0;
		size_t _channels = 0;
		size_t _samples_per_second = 0;
	};
}

#endif
