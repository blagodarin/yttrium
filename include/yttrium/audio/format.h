#ifndef _include_yttrium_audio_format_h_
#define _include_yttrium_audio_format_h_

#include <cstddef>

namespace Yttrium
{
	class AudioFormat
	{
	public:
		constexpr AudioFormat() noexcept = default;
		constexpr AudioFormat(size_t bytes_per_sample, size_t channels, size_t samples_per_second) noexcept
			: _bytes_per_sample{ bytes_per_sample }, _channels{ channels }, _samples_per_second{ samples_per_second } {}

		constexpr size_t block_size() const noexcept { return _bytes_per_sample * _channels; }
		constexpr size_t bytes_per_sample() const noexcept { return _bytes_per_sample; }
		constexpr size_t channels() const noexcept { return _channels; }
		constexpr size_t bytes_per_second() const noexcept { return _bytes_per_sample * _channels * _samples_per_second; }
		constexpr size_t samples_per_second() const noexcept { return _samples_per_second; }

	private:
		size_t _bytes_per_sample = 0;
		size_t _channels = 0;
		size_t _samples_per_second = 0;
	};
}

#endif
