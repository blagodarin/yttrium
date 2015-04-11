#ifndef SRC_IMAGE_DDS_H
#define SRC_IMAGE_DDS_H

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
