/// \file
/// \brief %Image input/output facilities.

#ifndef __Y_IMAGE_H
#define __Y_IMAGE_H

#include <yttrium/buffer.h>
#include <yttrium/size.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

class Image;

/// Pixel format.

enum class PixelFormat
{
	Gray,      ///< Gray.
	GrayAlpha, ///< Gray-alpha.
	AlphaGray, ///< Alpha-gray.
	Rgb,       ///< Red-green-blue.
	Bgr,       ///< Blue-green-red (reverse).
	Rgba,      ///< Red-green-blue-alpha.
	Bgra,      ///< Blue-green-red-alpha (reverse RGB).
	Argb,      ///< Alpha-red-green-blue (reverse alpha).
	Abgr,      ///< Alpha-blue-green-red (reverse).
};

/// Image file type.

enum class ImageType
{
	Auto, ///< Automatical detection.
	Tga,  ///< Truevision TARGA (TGA) file format.
	Png,  ///< Portable Network Graphics (PNG) file format (write only).
	Jpeg, ///< Joint Photographic Experts Group (JPEG) file format (read only).
	Dds,  ///< DirectDraw Surface (DDS) file format (read only).
};

/// Image orientation.

enum class ImageOrientation
{
	XRightYDown, ///< X left-to-right, Y top-to-bottom.
	XRightYUp,   ///< X left-to-right, Y bottom-to-top.
	XLeftYDown,  ///< X right-to-left, Y top-to-bottom.
	XLeftYUp,    ///< X right-to-left, Y bottom-to-top.
};

/// Image format.

class Y_API ImageFormat
{
	friend Image;

public:

	///
	ImageFormat();

public:

	///
	inline size_t bits_per_channel() const;

	///
	inline size_t bits_per_pixel() const;

	///
	inline size_t channels() const;

	///
	inline size_t frame_size() const;

	///
	inline size_t height() const;

	///
	inline ImageOrientation orientation() const;

	///
	inline PixelFormat pixel_format() const;

	///
	inline size_t row_alignment() const;

	///
	inline size_t row_size() const;

	///
	inline void set_height(size_t height);

	///
	inline void set_orientation(ImageOrientation orientation);

	///
	void set_pixel_format(PixelFormat pixel_format, size_t bits_per_pixel);

	///
	void set_row_alignment(size_t alignment);

	///
	void set_width(size_t width);

	///
	Size size() const { return Size(_width, _height); }

	///
	inline size_t width() const;

public:

	///
	inline bool operator ==(const ImageFormat& format) const;

	///
	inline bool operator !=(const ImageFormat& format) const;

private:

	PixelFormat      _pixel_format;
	size_t           _channels;
	size_t           _bits_per_pixel;
	ImageOrientation _orientation;
	size_t           _width;
	size_t           _row_alignment;
	size_t           _row_size;
	size_t           _height;
};

///

class Y_API Image
{
public:

	///

	Image(Allocator* allocator = DefaultAllocator)
		: _buffer(allocator)
	{
	}

	/// Allocate an image for the specified \a format.
	/// \param format Image format.
	/// \param allocator Image allocator.
	/// \note The image data is left uninitialized.

	Image(const ImageFormat& format, Allocator* allocator = DefaultAllocator);

public:

	///
	const void* const_data() const
	{
		return _buffer.data();
	}

	///
	void* data()
	{
		return _buffer.data();
	}

	///
	const void* data() const
	{
		return _buffer.data();
	}

	///
	ImageFormat format() const
	{
		return _format;
	}

	///
	bool intensity_to_bgra();

	///
	bool is_valid() const
	{
		return _buffer.data();
	}

	///
	bool load(const StaticString& name, ImageType type = ImageType::Auto);

	///
	bool save(const StaticString& name, ImageType type = ImageType::Auto) const;

	/// Change the format to \a format.
	/// \param format New image format.
	/// \note The image data becomes undefined after the call.
	void set_format(const ImageFormat& format);

	///
	void set_size(size_t width, size_t height, size_t row_alignment = 0);

	///
	bool swap_channels();

public:

	///

	bool operator ==(const Image& image) const;

private:

	ImageFormat _format;
	Buffer      _buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t ImageFormat::bits_per_channel() const
{
	return _bits_per_pixel % _channels ? 0 : _bits_per_pixel / _channels;
}

size_t ImageFormat::bits_per_pixel() const
{
	return _bits_per_pixel;
}

size_t ImageFormat::channels() const
{
	return _channels;
}

size_t ImageFormat::frame_size() const
{
	return _row_size * _height;
}

size_t ImageFormat::height() const
{
	return _height;
}

ImageOrientation ImageFormat::orientation() const
{
	return _orientation;
}

PixelFormat ImageFormat::pixel_format() const
{
	return _pixel_format;
}

size_t ImageFormat::row_alignment() const
{
	return _row_alignment;
}

size_t ImageFormat::row_size() const
{
	return _row_size;
}

void ImageFormat::set_height(size_t height)
{
	_height = height;
}

void ImageFormat::set_orientation(ImageOrientation orientation)
{
	_orientation = orientation;
}

size_t ImageFormat::width() const
{
	return _width;
}

bool ImageFormat::operator ==(const ImageFormat& format) const
{
	return _pixel_format == format._pixel_format
		&& _bits_per_pixel == format._bits_per_pixel
		&& _orientation == format._orientation
		&& _width == format._width
		&& _row_alignment == format._row_alignment
		&& _height == format._height;
}

bool ImageFormat::operator !=(const ImageFormat& format) const
{
	return _pixel_format != format._pixel_format
		|| _bits_per_pixel != format._bits_per_pixel
		|| _orientation != format._orientation
		|| _width != format._width
		|| _row_alignment != format._row_alignment
		|| _height != format._height;
}

} // namespace Yttrium

#endif // __Y_IMAGE_H
