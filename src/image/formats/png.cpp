#include "../formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/writer.h>

#include <png.h>

namespace
{
	using namespace Yttrium;

	bool can_write(const ImageFormat& format)
	{
		switch (format.pixel_format())
		{
		case PixelFormat::Gray:
			if (format.bits_per_pixel() != 8 && format.bits_per_pixel() != 16)
				return false;
			break;

		case PixelFormat::GrayAlpha:
		case PixelFormat::AlphaGray:
			if (format.bits_per_pixel() != 16 && format.bits_per_pixel() != 32)
				return false;
			break;

		case PixelFormat::Rgb:
		case PixelFormat::Bgr:
			if (format.bits_per_pixel() != 24 && format.bits_per_pixel() != 48)
				return false;
			break;

		case PixelFormat::Rgba:
		case PixelFormat::Bgra:
		case PixelFormat::Argb:
		case PixelFormat::Abgr:
			if (format.bits_per_pixel() != 32 && format.bits_per_pixel() != 64)
				return false;
			break;

		default:
			return false;
		}

		if (format.orientation() != ImageOrientation::XRightYDown && format.orientation() != ImageOrientation::XRightYUp)
			return false;

		if (format.width() <= 0 || format.width() > UINT32_MAX)
			return false;

		if (format.height() <= 0 || format.height() > UINT32_MAX)
			return false;

		return true;
	}

	class PngWriter
	{
	public:
		PngWriter(Writer& writer)
		{
			_png = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
			if (!_png)
				return;
			_info = ::png_create_info_struct(_png);
			if (!_info)
				return;
			::png_set_write_fn(_png, &writer, write_callback, flush_callback);
		}

		~PngWriter()
		{
			if (_png)
			{
				if (_info)
					::png_destroy_write_struct(&_png, &_info);
				else
					::png_destroy_write_struct(&_png, nullptr);
			}
		}

	private:
		static void write_callback(png_struct* png_ptr, png_byte* data, png_size_t length)
		{
			reinterpret_cast<Writer*>(::png_get_io_ptr(png_ptr))->write(data, length);
		}

		static void flush_callback(png_struct*)
		{
		}

	public:
		png_struct* _png = nullptr;
		png_info* _info = nullptr;
	};
}

namespace Yttrium
{
	bool write_png(Writer& writer, const ImageFormat& format, const void* data)
	{
		if (!::can_write(format))
			return false;

		int color_type = 0;
		int transforms = 0;

		if (format.pixel_format() == PixelFormat::AlphaGray
			|| format.pixel_format() == PixelFormat::Argb
			|| format.pixel_format() == PixelFormat::Abgr)
		{
			transforms |= PNG_TRANSFORM_SWAP_ALPHA;
		}

		if (format.pixel_format() == PixelFormat::Bgr
			|| format.pixel_format() == PixelFormat::Bgra
			|| format.pixel_format() == PixelFormat::Abgr)
		{
			transforms |= PNG_TRANSFORM_BGR;
		}

		switch (format.pixel_format())
		{
		case PixelFormat::Gray:
#ifndef Y_IS_BIG_ENDIAN
			if (format.bits_per_pixel() > 8)
				transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
			color_type = PNG_COLOR_TYPE_GRAY;
			break;

		case PixelFormat::GrayAlpha:
		case PixelFormat::AlphaGray:
#ifndef Y_IS_BIG_ENDIAN
			if (format.bits_per_pixel() > 16)
				transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;

		case PixelFormat::Rgb:
		case PixelFormat::Bgr:
#ifndef Y_IS_BIG_ENDIAN
			if (format.bits_per_pixel() > 24)
				transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
			color_type = PNG_COLOR_TYPE_RGB;
			break;

		case PixelFormat::Rgba:
		case PixelFormat::Bgra:
		case PixelFormat::Argb:
		case PixelFormat::Abgr:
#ifndef Y_IS_BIG_ENDIAN
			if (format.bits_per_pixel() > 32)
				transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		}

		PngWriter png_writer(writer);

		const auto rows = std::make_unique<png_bytep[]>(format.height());

		if (::setjmp(png_jmpbuf(png_writer._png)))
			return false;

		::png_set_compression_level(png_writer._png, 0);
		::png_set_IHDR(png_writer._png, png_writer._info, format.width(), format.height(), format.bits_per_channel(),
			color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		if (format.orientation() == ImageOrientation::XRightYDown)
		{
			for (size_t i = 0, j = 0; i < format.height(); i++, j += format.row_size())
				rows[i] = const_cast<png_bytep>(static_cast<png_const_bytep>(data) + j);
		}
		else
		{
			for (size_t i = format.height(), j = 0; i > 0; i--, j += format.row_size())
				rows[i - 1] = const_cast<png_bytep>(static_cast<png_const_bytep>(data) + j);
		}

		::png_set_rows(png_writer._png, png_writer._info, rows.get());
		::png_write_png(png_writer._png, png_writer._info, transforms, nullptr);

		return true;
	}
}
