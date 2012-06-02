#include "image.h"

#include <Yttrium/package.h>

#include "png.h"
#include "tga.h"

namespace Yttrium
{

ImageReader::ImageReader(const ImageReader &reader)
	: _private(Private::copy(reader._private))
{
}

void ImageReader::close()
{
	if (Private::should_free(&_private))
	{
		Private::free(&_private);
	}
}

ImageFormat ImageReader::format() const
{
	return _private
		? _private->_format
		: ImageFormat();
}

bool ImageReader::open(const StaticString &name, ImageType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case ImageType::Tga:

		_private = new(allocator->allocate<TgaReader>()) TgaReader(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();

			if (extension == ".tga")
			{
				_private = new(allocator->allocate<TgaReader>()) TgaReader(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, allocator)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	return false;
}

bool ImageReader::read(void *buffer)
{
	bool result = false;

	if (_private && !_private->_is_used)
	{
		result = _private->read(buffer, _private->_format.frame_size());
		_private->_is_used = true;
	}

	return result;
}

ImageReader &ImageReader::operator =(const ImageReader &reader)
{
	close();

	_private = Private::copy(reader._private);

	return *this;
}

ImageWriter::ImageWriter(const ImageWriter &reader)
	: _private(Private::copy(reader._private))
{
}

void ImageWriter::close()
{
	if (Private::should_free(&_private))
	{
		Private::free(&_private);
	}
}

bool ImageWriter::open(const StaticString &name, ImageType type, Allocator *allocator)
{
	close();

	switch (type)
	{
	case ImageType::Tga:

		_private = new(allocator->allocate<TgaWriter>()) TgaWriter(allocator);
		break;

	case ImageType::Png:

		_private = new(allocator->allocate<PngWriter>()) PngWriter(allocator);
		break;

	default:

		{
			StaticString extension = name.file_extension();

			if (extension == ".tga")
			{
				_private = new(allocator->allocate<TgaWriter>()) TgaWriter(allocator);
			}
			else if (extension == ".png")
			{
				_private = new(allocator->allocate<PngWriter>()) PngWriter(allocator);
			}
		}
		break;
	}

	if (_private)
	{
		if (_private->_file.open(name, File::Write)
			&& _private->open())
		{
			return true;
		}

		close();
	}

	return false;
}

ImageFormatFlags ImageWriter::set_format(const ImageFormat &format)
{
	if (!_private || _private->_is_ready || _private->_is_used)
	{
		return ImageFormat::AllFlags;
	}

	ImageFormatFlags result = _private->set_format(format);

	if (!result)
	{
		_private->_is_ready = true;
		_private->_format = format;
	}

	return result;
}

bool ImageWriter::write(const void *buffer)
{
	bool result = false;

	if (_private && _private->_is_ready && !_private->_is_used)
	{
		result = _private->write(buffer, _private->_format.frame_size());
		_private->_is_used = true;
	}

	return result;
}

ImageWriter &ImageWriter::operator =(const ImageWriter &writer)
{
	close();

	_private = Private::copy(writer._private);

	return *this;
}

bool ImageWriter::Private::open()
{
	return true;
}

} // namespace Yttrium
