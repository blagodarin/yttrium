/// \file
/// \brief %Image input/output facilities.

#ifndef _include_yttrium_image_h_
#define _include_yttrium_image_h_

#include <yttrium/math/size.h>
#include <yttrium/memory/buffer.h>

namespace Yttrium
{
	class Reader;
	class StaticString;
	class Writer;

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

	class Image;

	/// Image format.
	class Y_API ImageFormat
	{
		friend Image;

	public:
		///
		ImageFormat();

		///
		size_t bits_per_channel() const { return _bits_per_pixel % _channels ? 0 : _bits_per_pixel / _channels; }

		///
		size_t bits_per_pixel() const { return _bits_per_pixel; }

		///
		size_t channels() const { return _channels; }

		///
		size_t frame_size() const { return _row_size * _height; }

		///
		size_t height() const { return _height; }

		///
		ImageOrientation orientation() const { return _orientation; }

		///
		PixelFormat pixel_format() const { return _pixel_format; }

		///
		size_t row_alignment() const { return _row_alignment; }

		///
		size_t row_size() const { return _row_size; }

		///
		void set_height(size_t height) { _height = height; }

		///
		void set_orientation(ImageOrientation orientation) { _orientation = orientation; }

		///
		void set_pixel_format(PixelFormat pixel_format, size_t bits_per_pixel);

		///
		void set_row_alignment(size_t alignment);

		///
		void set_width(size_t width);

		///
		Size size() const { return Size(_width, _height); }

		///
		size_t width() const { return _width; }

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

	inline bool operator==(const ImageFormat& a, const ImageFormat& b)
	{
		return a.pixel_format() == b.pixel_format() && a.bits_per_pixel() == b.bits_per_pixel()
			&& a.orientation() == b.orientation() && a.width() == b.width() && a.row_size() == b.row_size()
			&& a.height() == b.height();
	}

	inline bool operator!=(const ImageFormat& a, const ImageFormat& b) { return !(a == b); }

	///
	class Y_API Image
	{
	public:
		///
		Image() = default;

		/// Allocate an image for the specified \a format.
		/// \param format Image format.
		/// \note The image data is left uninitialized.
		Image(const ImageFormat&);

		///
		void* data() noexcept { return _buffer.data(); }
		const void* data() const noexcept { return _buffer.data(); }

		///
		ImageFormat format() const { return _format; }

		///
		bool intensity_to_bgra();

		///
		bool load(Reader&&, ImageType = ImageType::Auto);

		///
		bool save(const StaticString& name, ImageType = ImageType::Auto) const;

		///
		bool save(Writer&&, ImageType) const;

		///
		void set_size(size_t width, size_t height, size_t row_alignment = 0);

		///
		bool swap_channels();

		///
		Buffer to_buffer(ImageType);

	private:
		ImageFormat _format;
		Buffer _buffer;
	};
}

#endif
