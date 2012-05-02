#ifndef __IMAGE_PNG_HPP
#define __IMAGE_PNG_HPP

#include <png.h>

#include "image.hpp"

namespace Yttrium
{

class PngWriter: public ImageWriter::Private
{
public:

	PngWriter(Allocator *allocator)
		: ImageWriter::Private(allocator)
	{
	}

	virtual ~PngWriter();

public:

	virtual bool open();

	virtual ImageFormatFlags set_format(const ImageFormat &format);

	virtual bool write(const void *buffer);

private:

	png_structp _png;
	png_infop   _info;
};

} // namespace Yttrium

#endif // __IMAGE_PNG_HPP
