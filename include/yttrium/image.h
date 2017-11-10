/// \file
/// \brief

#ifndef _include_yttrium_image_h_
#define _include_yttrium_image_h_

#include <yttrium/math/size.h>
#include <yttrium/memory/buffer.h>

#include <memory>
#include <optional>
#include <string>

namespace Yttrium
{
	class Source;
	class Writer;

	/// Pixel format.
	enum class PixelFormat
	{
		Gray,      ///< Gray.
		GrayAlpha, ///< Gray-alpha.
		Rgb,       ///< Red-green-blue.
		Bgr,       ///< Blue-green-red (reverse).
		Rgba,      ///< Red-green-blue-alpha.
		Bgra,      ///< Blue-green-red-alpha (reverse RGB).
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
	public:
		///
		ImageFormat(size_t width, size_t height, PixelFormat, size_t bits_per_pixel, size_t row_alignment = 1, ImageOrientation = ImageOrientation::XRightYDown);

		///
		ImageFormat(size_t width, size_t height, PixelFormat pixel_format, size_t bits_per_pixel, ImageOrientation orientation)
			: ImageFormat{width, height, pixel_format, bits_per_pixel, 1, orientation} {}

		///
		ImageFormat(const Size& size, PixelFormat pixel_format, size_t bits_per_pixel, size_t row_alignment = 1, ImageOrientation orientation = ImageOrientation::XRightYDown)
			: ImageFormat{static_cast<size_t>(size._width), static_cast<size_t>(size._height), pixel_format, bits_per_pixel, row_alignment, orientation} {}

		///
		ImageFormat(const Size& size, PixelFormat pixel_format, size_t bits_per_pixel, ImageOrientation orientation)
			: ImageFormat{static_cast<size_t>(size._width), static_cast<size_t>(size._height), pixel_format, bits_per_pixel, 1, orientation} {}

		///
		size_t bits_per_channel() const { return (_bits_per_pixel % _channels) ? 0 : _bits_per_pixel / _channels; }

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
		Size size() const { return {static_cast<int>(_width), static_cast<int>(_height)}; }

		///
		size_t width() const { return _width; }

	private:
		PixelFormat _pixel_format;
		size_t _channels;
		size_t _bits_per_pixel;
		ImageOrientation _orientation;
		size_t _width;
		size_t _row_alignment;
		size_t _row_size;
		size_t _height;
		friend class Image;
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
		static std::optional<Image> load(const Source&, ImageType = ImageType::Auto);

		/// Creates an image of the specified format with uninitialized contents.
		explicit Image(const ImageFormat& format);

		/// Creates an image of the specified format, initialized from the provided data.
		Image(const ImageFormat&, const void* data);

		///
		void* data() noexcept { return _buffer.data(); }
		const void* data() const noexcept { return _buffer.data(); }

		///
		ImageFormat format() const { return _format; }

		///
		bool save(const std::string& path, ImageType = ImageType::Auto) const;

		///
		bool save(Writer&&, ImageType) const;

		///
		bool swap_channels();

		///
		Buffer to_buffer(ImageType) const;

	private:
		ImageFormat _format;
		Buffer _buffer;
		Image(const ImageFormat& format, Buffer&& buffer) : _format(format), _buffer(std::move(buffer)) {}
	};

	///
	Y_API Image to_bgra(const Image&);

	///
	Y_API std::optional<Image> intensity_to_bgra(const Image&);
}

#endif
