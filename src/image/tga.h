#ifndef _src_image_tga_h_
#define _src_image_tga_h_

namespace Yttrium
{
	class Buffer;
	class File;
	class ImageFormat;
	class Reader;
	template <typename> class Writer;

	bool read_tga_header(Reader&, ImageFormat&);

	bool write_tga(Writer<Buffer>&, const ImageFormat&, const void*);
	bool write_tga(Writer<File>&, const ImageFormat&, const void*);
}

#endif
