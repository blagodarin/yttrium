#ifndef _src_image_dds_dds_h_
#define _src_image_dds_dds_h_

#include "../image.h"

namespace Yttrium
{
	class DdsReader : public ImageReader
	{
	public:
		DdsReader(const StaticString& name, Allocator&);

		bool open() override;
		bool read(void*) override;
	};
}

#endif
