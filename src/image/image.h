#ifndef __IMAGE_IMAGE_H
#define __IMAGE_IMAGE_H

#include <yttrium/image.h>

#include "../base/file.h"

namespace Yttrium
{

class ImageReader
{
public:

	virtual ~ImageReader();

	virtual bool open() = 0;
	virtual bool read(void* buffer) = 0;

public:

	Allocator*  _allocator;
	ImageFormat _format;
	File        _file;

protected:

	ImageReader(const StaticString& name, Allocator* allocator)
		: _allocator(allocator)
		, _file(name, allocator)
	{
	}
};

class ImageWriter
{
public:

	virtual ~ImageWriter();

	virtual bool open();
	virtual bool set_format(const ImageFormat& format) = 0;
	virtual bool write(const void* buffer) = 0;

public:

	Allocator*  _allocator;
	ImageFormat _format;
	File        _file;

protected:

	ImageWriter(const StaticString& name, Allocator* allocator)
		: _allocator(allocator)
		, _file(name, File::Write | File::Truncate, allocator)
	{
	}
};

} // namespace Yttrium

#endif // __IMAGE_IMAGE_H
