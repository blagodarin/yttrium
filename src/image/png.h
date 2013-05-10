#ifndef __IMAGE_PNG_H
#define __IMAGE_PNG_H

#include "image.h"

#include <png.h>

namespace Yttrium
{

class PngWriter
	: public ImageWriter
{
public:

	PngWriter(Allocator *allocator);
	~PngWriter() override;

public:

	bool open() override;
	bool set_format(const ImageFormat &format) override;
	bool write(const void *buffer) override;

private:

	png_structp _png;
	png_infop   _info;
};

} // namespace Yttrium

#endif // __IMAGE_PNG_H
