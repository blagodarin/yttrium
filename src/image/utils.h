#ifndef _src_image_utils_h_
#define _src_image_utils_h_

namespace Yttrium
{
	constexpr unsigned unaligned_image_row_size(unsigned width, unsigned bits_per_pixel)
	{
		return (width * bits_per_pixel + 7) / 8;
	}

	constexpr unsigned aligned_image_row_size(unsigned width, unsigned bits_per_pixel, unsigned row_alignment)
	{
		return (unaligned_image_row_size(width, bits_per_pixel) + row_alignment - 1) / row_alignment * row_alignment;
	}
}

#endif
