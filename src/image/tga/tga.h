#ifndef _src_image_tga_tga_h_
#define _src_image_tga_tga_h_

#include "../../io/writer.h"
#include "../image.h"

namespace Yttrium
{
	class TgaReader : public ImageReader
	{
	public:
		TgaReader(const StaticString& name, Allocator&);

		bool open() override;
		bool read(void*) override;
	};

	bool write_tga(Writer<Buffer>&, const ImageFormat&, const void*);
	bool write_tga(Writer<File>&, const ImageFormat&, const void*);
}

#endif
