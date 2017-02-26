#ifndef _tests_image_h_
#define _tests_image_h_

#include <yttrium/image.h>
#include "../src/image/utils.h"

#include <cstring>

namespace Yttrium
{
	inline bool operator==(const Image& a, const Image& b)
	{
		const auto format = a.format();
		if (format != b.format())
			return false;
		for (size_t i = 0; i < format.height(); ++i)
		{
			const auto a_row = static_cast<const uint8_t*>(a.data()) + i * format.row_size();
			const auto b_row = static_cast<const uint8_t*>(b.data()) + i * format.row_size();
			if (std::memcmp(a_row, b_row, unaligned_image_row_size(format.width(), format.bits_per_pixel())))
				return false;
		}
		return true;
	}

	inline bool operator!=(const Image& a, const Image& b)
	{
		return !(a == b);
	}
}

#endif
