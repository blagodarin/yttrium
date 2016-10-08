#ifndef _src_image_jpeg_h_
#define _src_image_jpeg_h_

namespace Yttrium
{
	class Buffer;
	class ImageFormat;
	class Reader;

	bool read_jpeg(Reader&, ImageFormat&, Buffer&);
}

#endif
