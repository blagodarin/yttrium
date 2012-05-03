/// \file
/// \brief %Image input/output facilities.

// TODO: Pixel format flags and image orientation flags are flawed.
// They are "old" enums which expose their values to the "global" namespace,
// but they can't also be "new" enums, because all the flags would require explicit integer casts.

#ifndef __Y_IMAGE_HPP
#define __Y_IMAGE_HPP

#include <Yttrium/allocator.hpp>
#include <Yttrium/static_string.hpp>

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
	Auto, ///< Automatical format detection (read only).
	Tga,  ///< Truevision TARGA (TGA) file format.
	Png,  ///< Portable Network Graphics (PNG) file format (write only).
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

class ImageFormat
{
public:

	enum: ImageFormatFlags
	{
		DepthFlag       = 1 << 0, ///< Depth flag.
		ChannelsFlag    = 1 << 1, ///< Channels flag.
		PixelFormatFlag = 1 << 2, ///< Pixel format flag.
		OrientationFlag = 1 << 3, ///< Orientation flag.
		WidthFlag       = 1 << 4, ///< Width flag.
		HeightFlag      = 1 << 5, ///< Height flag.

		/// All flags.

		AllFlags = DepthFlag | ChannelsFlag | PixelFormatFlag
			| OrientationFlag | WidthFlag | HeightFlag,
	};

public:

	uint_fast8_t     depth;        ///< Bytes per channel, usually 1.
	uint_fast8_t     channels;     ///< Channels per pixel.
	PixelFormat      pixel_format; ///< Pixel format.
	ImageOrientation orientation;  ///< Image orientation.
	size_t           width;        ///< Image width.
	size_t           height;       ///< Image height.

public:

	///

	size_t frame_size() const noexcept
	{
		return depth * channels * width * height;
	}
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

	ImageReader(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = HeapAllocator::instance()) noexcept
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

	bool open(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = HeapAllocator::instance()) noexcept;

	/// Read an image into \a buffer.
	/// \param buffer Buffer to read into.
	/// \return Success status.
	/// \note An image can only be read once

	bool read(void *buffer) noexcept;

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

	ImageWriter(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = HeapAllocator::instance()) noexcept
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

	bool open(const StaticString &name, ImageType type = ImageType::Auto, Allocator *allocator = HeapAllocator::instance()) noexcept;

	/// Set the format for the output image.
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

} // namespace Yttrium

#endif // __Y_IMAGE_HPP
