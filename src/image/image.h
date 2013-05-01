#ifndef __IMAGE_IMAGE_H
#define __IMAGE_IMAGE_H

#include <Yttrium/image.h>

#include "../base/file.h"

namespace Yttrium
{

class Y_PRIVATE ImageReader::Private
	: public PrivateBase<ImageReader::Private>
{
public:

	Private(ImageType type, Allocator *allocator)
		: PrivateBase(allocator)
		, _type(type)
		, _is_used(false)
	{
	}

	virtual ~Private() {}

public:

	virtual bool open() = 0;

	virtual bool read(void *buffer) = 0;

public:

	ImageType   _type;
	bool        _is_used;
	ImageFormat _format;
	File        _file;
};

class Y_PRIVATE ImageWriter::Private
	: public PrivateBase<ImageWriter::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
		, _is_ready(false)
		, _is_used(false)
	{
	}

	virtual ~Private() {}

public:

	virtual bool open();

	virtual ImageFormatFlags set_format(const ImageFormat &format) = 0;

	virtual bool write(const void *buffer) = 0;

public:

	bool        _is_ready;
	bool        _is_used;
	ImageFormat _format;
	StaticFile  _file;
};

} // namespace Yttrium

#endif // __IMAGE_IMAGE_H
