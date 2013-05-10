#ifndef __IMAGE_TGA_H
#define __IMAGE_TGA_H

#include "image.h"

namespace Yttrium
{

class TgaReader
	: public ImageReader
{
public:

	TgaReader(Allocator *allocator);

public:

	bool open() override;
	bool read(void *buffer) override;
};

class TgaWriter
	: public ImageWriter
{
public:

	TgaWriter(Allocator *allocator);

public:

	bool set_format(const ImageFormat &format) override;
	bool write(const void *buffer) override;
};

} // namespace Yttrium

#endif // __IMAGE_TGA_H
