#ifndef __IMAGE_IMAGE_H
#define __IMAGE_IMAGE_H

#include <yttrium/image.h>

#include "../base/file.h"

namespace Yttrium
{

class ImageReader
{
public:

	ImageReader(Allocator *allocator);
	virtual ~ImageReader();

public:

	virtual bool open() = 0;
	virtual bool read(void *buffer) = 0;

public:

	Allocator   *_allocator;
	ImageFormat  _format;
	StaticFile   _file;
};

class ImageWriter
{
public:

	ImageWriter(Allocator *allocator);
	virtual ~ImageWriter();

public:

	virtual bool open();
	virtual bool set_format(const ImageFormat &format) = 0;
	virtual bool write(const void *buffer) = 0;

public:

	Allocator   *_allocator;
	ImageFormat  _format;
	StaticFile   _file;
};

} // namespace Yttrium

#endif // __IMAGE_IMAGE_H
