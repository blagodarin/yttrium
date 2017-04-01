#include "formats.h"

#include <yttrium/image.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/source.h>
#include "../utils/fourcc.h"

#include <new>

namespace
{
	bool read_image_data(Yttrium::Reader& reader, const Yttrium::ImageFormat& format, Yttrium::Buffer& buffer)
	{
		const auto frame_size = format.frame_size();
		try
		{
			buffer.reset(frame_size);
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}
		return reader.read(buffer.data(), frame_size) == frame_size;
	}
}

namespace Yttrium
{
	boost::optional<ImageFormat> read_image(const Source& source, ImageType type, Buffer& buffer)
	{
		Reader reader{source};
		boost::optional<ImageFormat> format;
		switch (type)
		{
		case ImageType::Tga: format = read_tga_header(reader); break;
#ifndef Y_NO_JPEG
		case ImageType::Jpeg: return read_jpeg(source, buffer);
#endif
		case ImageType::Dds: format = read_dds_header(reader); break;
		default: return {};
		}
		if (!format || !read_image_data(reader, *format, buffer))
			return {};
		return format;
	}

	bool write_image(Writer& writer, ImageType type, const ImageFormat& format, const void* data)
	{
		switch (type)
		{
		case ImageType::Tga: return write_tga(writer, format, data);
#ifndef Y_NO_PNG
		case ImageType::Png: return write_png(writer, format, data);
#endif
		default: return false;
		}
	}

	bool detect_image_type(const Source& source, ImageType& type)
	{
		uint32_t signature = 0;
		if (!source.read_at(0, signature))
			return false;
		switch (signature)
		{
		case "DDS "_fourcc:
			type = ImageType::Dds;
			return true;
#ifndef Y_NO_PNG
		case "\xff\xd8\xff\xe0"_fourcc: // SOI marker and JFIF APP0 marker.
			type = ImageType::Jpeg;
			return true;
#endif
#ifndef Y_NO_JPEG
		case "\x89PNG"_fourcc:
			type = ImageType::Png;
			return true;
#endif
		default:
			// TGA is the last remaining option, and TGA images have no signature,
			// so we need to read the entire header anyway to determine whether it
			// is actually a TGA image.
			type = ImageType::Tga;
			return true;
		}
	}
}
