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
		Gray8,       ///< Gray.
		GrayAlpha16, ///< Gray-alpha.
		Rgb24,       ///< Red-green-blue.
		Bgr24,       ///< Blue-green-red (reverse).
		Rgba32,      ///< Red-green-blue-alpha.
		Bgra32,      ///< Blue-green-red-alpha (reverse RGB).
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
		ImageFormat(size_t width, size_t height, PixelFormat, size_t row_alignment = 1, ImageOrientation = ImageOrientation::XRightYDown);

		///
		ImageFormat(size_t width, size_t height, PixelFormat pixel_format, ImageOrientation orientation)
			: ImageFormat{width, height, pixel_format, 1, orientation} {}

		///
		ImageFormat(const Size& size, PixelFormat pixel_format, size_t row_alignment = 1, ImageOrientation orientation = ImageOrientation::XRightYDown)
			: ImageFormat{static_cast<size_t>(size._width), static_cast<size_t>(size._height), pixel_format, row_alignment, orientation} {}

		///
		ImageFormat(const Size& size, PixelFormat pixel_format, ImageOrientation orientation)
			: ImageFormat{static_cast<size_t>(size._width), static_cast<size_t>(size._height), pixel_format, 1, orientation} {}

		size_t bits_per_channel() const noexcept { return bits_per_pixel() / _channels; }
		size_t bits_per_pixel() const noexcept { return pixel_size() * 8; }
		size_t channels() const noexcept { return _channels; }
		size_t frame_size() const noexcept { return _row_size * _height; }
		size_t height() const noexcept { return _height; }
		ImageOrientation orientation() const noexcept { return _orientation; }
		PixelFormat pixel_format() const noexcept { return _pixel_format; }
		size_t pixel_size() const noexcept { return pixel_size(_pixel_format); }
		size_t row_alignment() const noexcept { return _row_alignment; }
		size_t row_size() const noexcept { return _row_size; }
		Size size() const noexcept { return {static_cast<int>(_width), static_cast<int>(_height)}; }
		size_t width() const noexcept { return _width; }

		///
		static constexpr size_t pixel_size(PixelFormat format) noexcept
		{
			switch (format)
			{
			case PixelFormat::Gray8: return 1;
			case PixelFormat::GrayAlpha16: return 2;
			case PixelFormat::Rgb24: return 3;
			case PixelFormat::Bgr24: return 3;
			case PixelFormat::Rgba32: return 4;
			case PixelFormat::Bgra32: return 4;
			}
			return 0;
		}

	private:
		PixelFormat _pixel_format;
		size_t _channels;
		ImageOrientation _orientation;
		size_t _width;
		size_t _row_alignment;
		size_t _row_size;
		size_t _height;
		friend class Image;
	};

	inline bool operator==(const ImageFormat& a, const ImageFormat& b) noexcept
	{
		return a.pixel_format() == b.pixel_format()
			&& a.orientation() == b.orientation()
			&& a.width() == b.width()
			&& a.row_size() == b.row_size()
			&& a.height() == b.height();
	}

	inline bool operator!=(const ImageFormat& a, const ImageFormat& b) noexcept { return !(a == b); }

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
		bool swap_channels() noexcept;

		///
		Buffer to_buffer(ImageType) const;

	private:
		ImageFormat _format;
		Buffer _buffer;
		Image(const ImageFormat& format, Buffer&& buffer) : _format{format}, _buffer{std::move(buffer)} {}
	};

	///
	Y_API Image to_bgra(const Image&);

	///
	Y_API std::optional<Image> intensity_to_bgra(const Image&);
}

#endif
