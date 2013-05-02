#ifndef __IMAGE_PROCESSING_H
#define __IMAGE_PROCESSING_H

#include <Yttrium/types.h>

namespace Yttrium
{

namespace ImageProcessing
{

void xyz8_to_zyx8(void *buffer, size_t width, size_t row_size, size_t height);

} // namespace ImageProcessing

} // namespace Yttrium

#endif // __IMAGE_PROCESSING_H
