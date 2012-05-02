#ifndef __IMAGE_IMAGE_HPP
#define __IMAGE_IMAGE_HPP

#include <Yttrium/image.hpp>

#include "../base/file.hpp"

namespace Yttrium
{

class ImageReader::Private: public PrivateBase<ImageReader::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
		, _is_used(false)
		, _size(0)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual bool open() = 0;

	virtual bool read(void *buffer) = 0;

public:

	bool        _is_used;
	ImageFormat _format;
	File        _file;

protected:

	size_t _size;
};

class ImageWriter::Private: public PrivateBase<ImageWriter::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
		, _is_ready(false)
		, _is_used(false)
		, _size(0)
	{
	}

	virtual ~Private()
	{
	}

public:

	virtual bool open();

	virtual ImageFormatFlags set_format(const ImageFormat &format) = 0;

	virtual bool write(const void *buffer) = 0;

public:

	bool        _is_ready;
	bool        _is_used;
	ImageFormat _format;
	StaticFile  _file;
	size_t      _size;
};

} // namespace Yttrium

#endif // __IMAGE_IMAGE_HPP
