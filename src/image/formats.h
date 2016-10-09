#ifndef _src_image_formats_h_
#define _src_image_formats_h_

namespace Yttrium
{
	class Allocator;
	class Buffer;
	class File;
	class ImageFormat;
	enum class ImageType;
	class Reader;
	template <typename> class Writer;

	bool read_dds_header(Reader&, ImageFormat&);

#ifndef Y_NO_JPEG
	bool read_jpeg(Reader&, ImageFormat&, Buffer&);
#endif

#ifndef Y_NO_PNG
	bool write_png(Writer<Buffer>&, const ImageFormat&, const void*);
	bool write_png(Writer<File>&, const ImageFormat&, const void*);
#endif

	bool read_tga_header(Reader&, ImageFormat&);
	bool write_tga(Writer<Buffer>&, const ImageFormat&, const void*);
	bool write_tga(Writer<File>&, const ImageFormat&, const void*);

	bool read_image(Reader&, ImageType, ImageFormat&, Buffer&);
	bool read_image_data(Reader&, const ImageFormat&, Buffer&);
	bool write_image(Buffer&, ImageType, const ImageFormat&, const void*);
	bool write_image(File&, ImageType, const ImageFormat&, const void*);

	bool detect_image_type(const Reader&, ImageType&);
}

#endif
