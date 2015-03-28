#ifndef __IMAGE_JPEG_JPEG_H
#define __IMAGE_JPEG_JPEG_H

#include "../image.h"

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>
#include <setjmp.h>

namespace Yttrium
{

class JpegReader: public ImageReader
{
public:

	JpegReader(const StaticString& name, Allocator* allocator);
	~JpegReader() override;

	bool open() override;
	bool read(void* buffer) override;

private:

	struct JpegErrorHandler
	{
		jpeg_error_mgr pub;
		jmp_buf        setjmp_buffer;
	};

private:

	static void error_callback(jpeg_common_struct* cinfo);

private:

	Buffer                 _buffer;
	jpeg_decompress_struct _decompressor;
	JpegErrorHandler       _error_handler;
};

} // namespace Yttrium

#endif // __IMAGE_JPEG_JPEG_H