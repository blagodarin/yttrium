#ifndef _src_image_formats_h_
#define _src_image_formats_h_

#include "../config.h"

#include <boost/optional/optional_fwd.hpp>

namespace Yttrium
{
	class Buffer;
	class ImageFormat;
	enum class ImageType;
	class Reader;
	class Writer;

	boost::optional<ImageFormat> read_dds_header(Reader&);

#ifndef Y_NO_JPEG
	boost::optional<ImageFormat> read_jpeg(Reader&, Buffer&);
#endif

#ifndef Y_NO_PNG
	bool write_png(Writer&, const ImageFormat&, const void*);
#endif

	boost::optional<ImageFormat> read_tga_header(Reader&);
	bool write_tga(Writer&, const ImageFormat&, const void*);

	boost::optional<ImageFormat> read_image(Reader&, ImageType, Buffer&);
	bool read_image_data(Reader&, const ImageFormat&, Buffer&);
	bool write_image(Writer&, ImageType, const ImageFormat&, const void*);

	bool detect_image_type(const Reader&, ImageType&);
}

#endif
