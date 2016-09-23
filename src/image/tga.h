#ifndef _src_image_tga_h_
#define _src_image_tga_h_

#include "image.h"

namespace Yttrium
{
	class TgaReader : public ImageReader
	{
	public:
		TgaReader(const StaticString& name, Allocator&);

		bool open() override;
		bool read(void*) override;
	};

	class TgaWriter : public ImageWriter
	{
	public:
		TgaWriter(const StaticString& name, Allocator&);

		bool set_format(const ImageFormat&) override;
		bool write(const void*) override;
	};
}

#endif
