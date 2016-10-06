#ifndef _src_image_png_png_h_
#define _src_image_png_png_h_

#include "../../io/writer.h"

namespace Yttrium
{
	class Allocator;
	class ImageFormat;

	bool write_png(Writer<Buffer>&, const ImageFormat&, const void*, Allocator&);
	bool write_png(Writer<File>&, const ImageFormat&, const void*, Allocator&);
}

#endif
