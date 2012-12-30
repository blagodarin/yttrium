/// \file
/// \brief %Image input/output facilities.

// TODO: Pixel format flags and image orientation flags are flawed.
// They are "old" enums which expose their values to the "global" namespace,
// but they can't also be "new" enums, because all the flags would require explicit integer casts.

#ifndef __Y_IMAGE_H
#define __Y_IMAGE_H

#include <Yttrium/static_string.h>

namespace Yttrium
{

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

/// Pixel format flags.

typedef uint_fast16_t PixelFormatFlags;

/// Pixel format flag.

enum: PixelFormatFlags
{
	GrayFlag      = 1 << 0, ///< Gray flag.
	GrayAlphaFlag = 1 << 1, ///< Gray-alpha flag.
	AlphaGrayFlag = 1 << 2, ///< Alpha-gray flag.
	RgbFlag       = 1 << 3, ///< RGB flag.
	BgrFlag       = 1 << 4, ///< BGR flag.
	RgbaFlag      = 1 << 5, ///< RGBA flag.
	BgraFlag      = 1 << 6, ///< BGRA flag.
	ArgbFlag      = 1 << 7, ///< ARGB flag.
	AbgrFlag      = 1 << 8, ///< ABGR flag.
};

/// Image file type.

enum class ImageType
{
	Auto, ///< Automatical detection.
	Tga,  ///< Truevision TARGA (TGA) file format.
	Png,  ///< Portable Network Graphics (PNG) file format (write only).
	Jpeg, ///< Joint Photographic Experts Group (JPEG) file format (read only).
};

/// Image orientation.

enum class ImageOrientation
{
	XRightYDown, ///< X left-to-right, Y top-to-bottom.
	XRightYUp,   ///< X left-to-right, Y bottom-to-top.
	XLeftYDown,  ///< X right-to-left, Y top-to-bottom.
	XLeftYUp,    ///< X right-to-left, Y bottom-to-top.
};

/// Image orientation flags.

typedef uint_fast8_t ImageOrientationFlags;

enum: ImageOrientationFlags
{
	XRightYDownFlag = 1 << 0, ///< X left-to-right, Y top-to-bottom.
	XRightYUpFlag   = 1 << 1, ///< X left-to-right, Y bottom-to-top.
	XLeftYDownFlag  = 1 << 2, ///< X right-to-left, Y top-to-bottom.
	XLeftYUpFlag    = 1 << 3, ///< X right-to-left, Y bottom-to-top.
};

/// Image format flags.

typedef uint_fast8_t ImageFormatFlags;

/// Image format.

class Y_API ImageFormat
{
public:

	enum: ImageFormatFlags
	{
		PixelFormatFlag  = 1 << 0, ///< Pixel format flag.
		BitsPerPixelFlag = 1 << 1, ///< Bits per pixel flag.
		OrientationFlag  = 1 << 2, ///< Orientation flag.
		WidthFlag        = 1 << 3, ///< Width flag.
		HeightFlag       = 1 << 4, ///< Height flag.

		/// All flags.

		AllFlags = PixelFormatFlag | BitsPerPixelFlag | OrientationFlag | WidthFlag | HeightFlag,
	};

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

	inline size_t row_size() const noexcept;

	///

	inline void set_height(size_t height) noexcept;

	///

	inline void set_orientation(ImageOrientation orientation) noexcept;

	///

	void set_pixel_format(PixelFormat pixel_format, size_t bits_per_pixel) noexcept;

	///

	void set_row_alignment(size_t row_alignment) noexcept;

	///

	inline void set_width(size_t width) noexcept;

	///

	inline size_t width() const noexcept;

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

/// Read-only audio file access interface.

class Y_API ImageReader
{
public:

	///

	ImageReader() noexcept
		: _private(nullptr)
	{
	}

	///

	ImageReader(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = DefaultAllocator) noexcept
		//: ImageReader() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(name, type, allocator);
	}

	///

	ImageReader(const ImageReader &reader) noexcept;

	///

	~ImageReader() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	/// Return the audio format.
	/// \return %Audio format.

	ImageFormat format() const noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	bool open(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = DefaultAllocator) noexcept;

	/// Read an image into \a buffer.
	/// \param buffer Buffer to read into.
	/// \return Success status.
	/// \note An image can only be read once

	bool read(void *buffer) noexcept;

	///

	ImageType type() const noexcept;

public:

	///

	ImageReader &operator =(const ImageReader &reader) noexcept;

public:

	class Private;

private:

	Private *_private;
};

/// Read-only audio file access interface.

class Y_API ImageWriter
{
public:

	///

	ImageWriter() noexcept
		: _private(nullptr)
	{
	}

	///

	ImageWriter(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = DefaultAllocator) noexcept
		//: ImageWriter() // TODO: Uncomment.
		: _private(nullptr)
	{
		open(name, type, allocator);
	}

	///

	ImageWriter(const ImageWriter &writer) noexcept;

	///

	~ImageWriter() noexcept
	{
		close();
	}

public:

	///

	void close() noexcept;

	///

	bool is_opened() const noexcept
	{
		return _private;
	}

	///

	bool open(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = DefaultAllocator) noexcept;

	/// Set the format for the output image.
	/// \param format The desired format.
	/// \return Flags for invalid format fields or 0 on success.

	ImageFormatFlags set_format(const ImageFormat &format) noexcept;

	/// Write the image from a buffer to the file.
	/// \param buffer A buffer to write from.
	/// \return Success status.

	bool write(const void *buffer) noexcept;

public:

	///

	ImageWriter &operator =(const ImageWriter &writer) noexcept;

public:

	class Private;

private:

	Private *_private;
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

void ImageFormat::set_width(size_t width) noexcept
{
	_width = width;
	set_row_alignment(_row_alignment);
}

size_t ImageFormat::width() const noexcept
{
	return _width;
}

} // namespace Yttrium

#endif // __Y_IMAGE_H
