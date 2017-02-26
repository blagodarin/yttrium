#ifndef _src_image_utils_h_
#define _src_image_utils_h_

#include <cstddef>

namespace Yttrium
{
	constexpr size_t unaligned_image_row_size(size_t width, size_t bits_per_pixel)
	{
		return (width * bits_per_pixel + 7) / 8;
	}

	constexpr size_t aligned_image_row_size(size_t width, size_t bits_per_pixel, size_t row_alignment)
	{
		return (unaligned_image_row_size(width, bits_per_pixel) + row_alignment - 1) / row_alignment * row_alignment;
	}
}

#endif
