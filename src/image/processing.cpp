#include "processing.h"

namespace Yttrium
{

namespace ImageProcessing
{

void xyz8_to_zyx8(void *buffer, size_t width, size_t row_size, size_t height)
{
	unsigned char *scanline = static_cast<unsigned char *>(buffer);

	for (size_t row = 0; row < height; ++row)
	{
		for (size_t offset = 0; offset < width * 3; offset += 3)
		{
			unsigned char x = scanline[offset];
			scanline[offset] = scanline[offset + 2];
			scanline[offset + 2] = x;
		}
		scanline += row_size;
	}
}

} // namespace ImageProcessing

} // namespace Yttrium
