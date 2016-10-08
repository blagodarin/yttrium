#ifndef _src_image_dds_h_
#define _src_image_dds_h_

namespace Yttrium
{
	class ImageFormat;
	class Reader;

	bool read_dds_header(Reader&, ImageFormat&);
}

#endif
