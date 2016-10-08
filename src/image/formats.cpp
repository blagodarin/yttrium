#include "formats.h"

#include <yttrium/image.h>
#include <yttrium/io/reader.h>
#include "../io/writer.h"

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
		default: return false; // TODO: Make it work for ImageType::Auto.
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

	bool write_image(Buffer& buffer, ImageType type, const ImageFormat& format, const void* data, Allocator& allocator)
	{
		Writer<Buffer> writer(buffer);
		switch (type)
		{
		case ImageType::Tga: return write_tga(writer, format, data);
	#ifndef Y_NO_PNG
		case ImageType::Png: return write_png(writer, format, data, allocator);
	#endif
		default: return false;
		}
	}

	bool write_image(File& file, ImageType type, const ImageFormat& format, const void* data, Allocator& allocator)
	{
		Writer<File> writer(file);
		switch (type)
		{
		case ImageType::Tga: return write_tga(writer, format, data);
	#ifndef Y_NO_PNG
		case ImageType::Png: return write_png(writer, format, data, allocator);
	#endif
		default: return false;
		}
	}
}
