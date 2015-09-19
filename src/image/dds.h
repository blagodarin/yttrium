#ifndef _src_image_dds_h_
#define _src_image_dds_h_

#include "image.h"

namespace Yttrium
{
	class DdsReader: public ImageReader
	{
	public:

		DdsReader(const StaticString& name, Allocator* allocator);

		bool open() override;
		bool read(void* buffer) override;
	};
}

#endif
