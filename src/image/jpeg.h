#ifndef __IMAGE_JPEG_H
#define __IMAGE_JPEG_H

#include <Yttrium/buffer.h>

#include "image.h"

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>
#include <setjmp.h>

namespace Yttrium
{

class JpegReader
	: public ImageReader::Private
{
public:

	JpegReader(Allocator *allocator);

	~JpegReader() override;

public:

	bool open() override;

	bool read(void *buffer, size_t frame_size) override;

private:

	struct JpegErrorHandler
	{
		jpeg_error_mgr pub;
		jmp_buf        setjmp_buffer;
	};

private:

	static void error_callback(jpeg_common_struct *cinfo);

private:

	Buffer                 _buffer;
	jpeg_decompress_struct _decompressor;
	JpegErrorHandler       _error_handler;
};

} // namespace Yttrium

#endif // __IMAGE_JPEG_H
