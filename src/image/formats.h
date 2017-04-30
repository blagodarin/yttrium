#ifndef _src_image_formats_h_
#define _src_image_formats_h_

#include <yttrium/std_optional.h>
#include "../config.h"

namespace Yttrium
{
	class Buffer;
	class ImageFormat;
	enum class ImageType;
	class Reader;
	class Source;
	class Writer;

	std::optional<ImageFormat> read_dds_header(Reader&);

#ifndef Y_NO_JPEG
	std::optional<ImageFormat> read_jpeg(const Source&, Buffer&);
#endif

#ifndef Y_NO_PNG
	bool write_png(Writer&, const ImageFormat&, const void*);
#endif

	std::optional<ImageFormat> read_tga_header(Reader&);
	bool write_tga(Writer&, const ImageFormat&, const void*);

	std::optional<ImageFormat> read_image(const Source&, ImageType, Buffer&);
	bool write_image(Writer&, ImageType, const ImageFormat&, const void*);

	bool detect_image_type(const Source&, ImageType&);
}

#endif
