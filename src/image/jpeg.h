#ifndef _src_image_jpeg_jpeg_h_
#define _src_image_jpeg_jpeg_h_

namespace Yttrium
{
	class Buffer;
	class File;
	class ImageFormat;

	bool read_jpeg(File&, ImageFormat&, Buffer&);
}

#endif
