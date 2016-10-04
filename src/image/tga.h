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

	bool write_tga(File&, const ImageFormat&, const void*);
}

#endif
