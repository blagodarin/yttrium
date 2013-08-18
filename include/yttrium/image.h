/// \file
/// \brief %Image input/output facilities.

#ifndef __Y_IMAGE_H
#define __Y_IMAGE_H

#include <yttrium/buffer.h>
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

	ImageFormat() noexcept;

public:

	///

	inline size_t bits_per_channel() const noexcept;

	///

	inline size_t bits_per_pixel() const noexcept;

	///

	inline size_t channels() const noexcept;

	///

	inline size_t frame_size() const noexcept;

	///

	inline size_t height() const noexcept;

	///

	inline ImageOrientation orientation() const noexcept;

	///

	inline PixelFormat pixel_format() const noexcept;

	///

	inline size_t row_alignment() const noexcept;

	///

	inline size_t row_size() const noexcept;

	///

	inline void set_height(size_t height) noexcept;

	///

	inline void set_orientation(ImageOrientation orientation) noexcept;

	///

	void set_pixel_format(PixelFormat pixel_format, size_t bits_per_pixel) noexcept;

	///

	void set_row_alignment(size_t alignment) noexcept;

	///

	void set_width(size_t width) noexcept;

	///

	inline size_t width() const noexcept;

public:

	///

	inline bool operator ==(const ImageFormat &format) const noexcept;

	///

	inline bool operator !=(const ImageFormat &format) const noexcept;

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

	inline Image(Allocator *allocator = DefaultAllocator) noexcept;

	/// Allocate an image for the specified \a format.
	/// \param format Image format.
	/// \param allocator Image allocator.
	/// \note The image data is left uninitialized.

	Image(const ImageFormat &format, Allocator *allocator = DefaultAllocator) noexcept;

public:

	///

	inline const void *const_data() const noexcept;

	///

	inline void *data() noexcept;

	///

	inline const void *data() const noexcept;

	///

	inline ImageFormat format() const noexcept;

	///

	bool intensity_to_bgra() noexcept;

	///

	inline bool is_valid() const noexcept;

	///

	bool load(const StaticString &name, ImageType type = ImageType::Auto) noexcept;

	///

	bool save(const StaticString &name, ImageType type = ImageType::Auto) const noexcept;

	/// Change the format to \a format.
	/// \param format New image format.
	/// \note The image data becomes undefined after the call.

	void set_format(const ImageFormat &format) noexcept;

	///

	void set_size(size_t width, size_t height, size_t row_alignment = 0) noexcept;

	///

	bool swap_channels() noexcept;

public:

	///

	bool operator ==(const Image &image) const noexcept;

private:

	ImageFormat _format;
	Buffer      _buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t ImageFormat::bits_per_channel() const noexcept
{
	return _bits_per_pixel % _channels ? 0 : _bits_per_pixel / _channels;
}

size_t ImageFormat::bits_per_pixel() const noexcept
{
	return _bits_per_pixel;
}

size_t ImageFormat::channels() const noexcept
{
	return _channels;
}

size_t ImageFormat::frame_size() const noexcept
{
	return _row_size * _height;
}

size_t ImageFormat::height() const noexcept
{
	return _height;
}

ImageOrientation ImageFormat::orientation() const noexcept
{
	return _orientation;
}

PixelFormat ImageFormat::pixel_format() const noexcept
{
	return _pixel_format;
}

size_t ImageFormat::row_alignment() const noexcept
{
	return _row_alignment;
}

size_t ImageFormat::row_size() const noexcept
{
	return _row_size;
}

void ImageFormat::set_height(size_t height) noexcept
{
	_height = height;
}

void ImageFormat::set_orientation(ImageOrientation orientation) noexcept
{
	_orientation = orientation;
}

size_t ImageFormat::width() const noexcept
{
	return _width;
}

bool ImageFormat::operator ==(const ImageFormat &format) const noexcept
{
	return _pixel_format == format._pixel_format
		&& _bits_per_pixel == format._bits_per_pixel
		&& _orientation == format._orientation
		&& _width == format._width
		&& _row_alignment == format._row_alignment
		&& _height == format._height;
}

bool ImageFormat::operator !=(const ImageFormat &format) const noexcept
{
	return _pixel_format != format._pixel_format
		|| _bits_per_pixel != format._bits_per_pixel
		|| _orientation != format._orientation
		|| _width != format._width
		|| _row_alignment != format._row_alignment
		|| _height != format._height;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Image::Image(Allocator *allocator) noexcept
	: _buffer(allocator)
{
}

const void *Image::const_data() const noexcept
{
	return _buffer.data();
}

void *Image::data() noexcept
{
	return _buffer.data();
}

const void *Image::data() const noexcept
{
	return _buffer.data();
}

ImageFormat Image::format() const noexcept
{
	return _format;
}

bool Image::is_valid() const noexcept
{
	return _buffer.data();
}

} // namespace Yttrium

#endif // __Y_IMAGE_H
