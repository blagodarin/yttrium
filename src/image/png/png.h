#ifndef _src_image_png_png_h_
#define _src_image_png_png_h_

namespace Yttrium
{
	class Allocator;
	class File;
	class ImageFormat;

	bool write_png(File&, const ImageFormat&, const void*, Allocator&);
}

#endif
