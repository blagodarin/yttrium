#ifndef _src_image_jpeg_jpeg_h_
#define _src_image_jpeg_jpeg_h_

#include "../image.h"

#include <cstdio> // <jpeglib.h> requires FILE declaration.
#include <jpeglib.h>
#include <setjmp.h>

namespace Yttrium
{
	class JpegReader : public ImageReader
	{
	public:
		JpegReader(const StaticString& name, Allocator&);
		~JpegReader() override;

		bool open() override;
		bool read(void*) override;

	private:
		static void error_callback(jpeg_common_struct*);

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
