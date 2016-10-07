#ifndef _src_image_dds_h_
#define _src_image_dds_h_

namespace Yttrium
{
	class File;
	class ImageFormat;

	bool read_dds_header(File&, ImageFormat&);
}

#endif
