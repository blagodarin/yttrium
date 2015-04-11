#ifndef SRC_IMAGE_JPEG_JPEG_H
#define SRC_IMAGE_JPEG_JPEG_H

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

		static void error_callback(jpeg_common_struct* cinfo);

	private:

		struct JpegErrorHandler
		{
			jpeg_error_mgr pub;
			jmp_buf        setjmp_buffer;
		};

		Buffer                 _buffer;
		jpeg_decompress_struct _decompressor;
		JpegErrorHandler       _error_handler;
	};
}

#endif
