#ifndef __IMAGE_TGA_H
#define __IMAGE_TGA_H

#include "image.h"

namespace Yttrium
{

class TgaReader: public ImageReader::Private
{
public:

	TgaReader(Allocator *allocator)
		: ImageReader::Private(ImageType::Tga, allocator)
	{
	}

public:

	virtual bool open();

	virtual bool read(void *buffer, size_t frame_size);
};

class TgaWriter: public ImageWriter::Private
{
public:

	TgaWriter(Allocator *allocator)
		: ImageWriter::Private(allocator)
	{
	}

public:

	virtual ImageFormatFlags set_format(const ImageFormat &format);

	virtual bool write(const void *buffer, size_t frame_size);
};

} // namespace Yttrium

#endif // __IMAGE_TGA_H
