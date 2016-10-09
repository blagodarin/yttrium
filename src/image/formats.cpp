#include "formats.h"

#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include "../utils/fourcc.h"

#include <new>

namespace Yttrium
{
	bool read_image(Reader& reader, ImageType type, ImageFormat& format, Buffer& buffer)
	{
		switch (type)
		{
		case ImageType::Tga: return read_tga_header(reader, format) && read_image_data(reader, format, buffer);
	#ifndef Y_NO_JPEG
		case ImageType::Jpeg: return read_jpeg(reader, format, buffer);
	#endif
		case ImageType::Dds: return read_dds_header(reader, format) && read_image_data(reader, format, buffer);
		default: return false;
		}
	}

	bool read_image_data(Reader& reader, const ImageFormat& format, Buffer& buffer)
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

	bool detect_image_type(const Reader& reader, ImageType& type)
	{
		uint32_t signature = 0;
		if (!reader.read_at(0, signature))
			return false;
		switch (signature)
		{
		case "DDS "_fourcc:
			type = ImageType::Dds;
			return true;
		case "\xff\xd8\xff\xe0"_fourcc: // SOI marker and JFIF APP0 marker.
			type = ImageType::Jpeg;
			return true;
		case "\x89PNG"_fourcc:
			type = ImageType::Png;
			return true;
		default:
			// TGA is the last remaining option, and TGA images have no signature,
			// so we need to read the entire header anyway to determine whether it
			// is actually a TGA image.
			type = ImageType::Tga;
			return true;
		}
	}
}
