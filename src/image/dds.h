#ifndef __IMAGE_DDS_H
#define __IMAGE_DDS_H

#include "image.h"

namespace Yttrium
{

class DdsReader
	: public ImageReader
{
public:

	DdsReader(Allocator *allocator);

public:

	bool open() override;
	bool read(void *buffer) override;
};

} // namespace Yttrium

#endif // __IMAGE_DDS_H
