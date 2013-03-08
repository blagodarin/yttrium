#ifndef __IMAGE_PNG_H
#define __IMAGE_PNG_H

#include "image.h"

#include <png.h>

namespace Yttrium
{

class PngWriter: public ImageWriter::Private
{
public:

	PngWriter(Allocator *allocator)
		: ImageWriter::Private(allocator)
	{
	}

	~PngWriter() override;

public:

	bool open() override;

	ImageFormatFlags set_format(const ImageFormat &format) override;

	bool write(const void *buffer, size_t frame_size) override;

private:

	png_structp _png;
	png_infop   _info;
};

} // namespace Yttrium

#endif // __IMAGE_PNG_H
