#ifndef SRC_IMAGE_TGA_H
#define SRC_IMAGE_TGA_H

#include "image.h"

namespace Yttrium
{
	class TgaReader: public ImageReader
	{
	public:

		TgaReader(const StaticString& name, Allocator* allocator);

		bool open() override;
		bool read(void* buffer) override;
	};

	class TgaWriter: public ImageWriter
	{
	public:

		TgaWriter(const StaticString& name, Allocator* allocator);

		bool set_format(const ImageFormat& format) override;
		bool write(const void* buffer) override;
	};
}

#endif
