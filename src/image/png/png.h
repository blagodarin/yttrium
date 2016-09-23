#ifndef _src_image_png_png_h_
#define _src_image_png_png_h_

#include "../image.h"

#include <png.h>

namespace Yttrium
{
	class PngWriter : public ImageWriter
	{
	public:
		PngWriter(const StaticString& name, Allocator&);
		~PngWriter() override;

		bool open() override;
		bool set_format(const ImageFormat&) override;
		bool write(const void*) override;

	private:
		png_struct* _png = nullptr;
		png_info* _info = nullptr;
	};
}

#endif
