#ifndef _src_image_image_h_
#define _src_image_image_h_

#include <yttrium/image.h>

#include <yttrium/file.h>

namespace Yttrium
{
	class ImageReader
	{
	public:
		virtual ~ImageReader() = default;

		virtual bool open() = 0;
		virtual bool read(void*) = 0;

	public:
		Allocator& _allocator;
		ImageFormat _format;
		File _file;

	protected:
		ImageReader(const StaticString& name, Allocator& allocator)
			: _allocator(allocator)
			, _file(name, allocator)
		{
		}
	};

	class ImageWriter
	{
	public:
		virtual ~ImageWriter() = default;

		virtual bool open();
		virtual bool set_format(const ImageFormat&) = 0;
		virtual bool write(const void*) = 0;

	public:
		Allocator& _allocator;
		ImageFormat _format;
		File _file;

	protected:
		ImageWriter(const StaticString& name, Allocator& allocator)
			: _allocator(allocator)
			, _file(name, File::Write | File::Truncate, allocator)
		{
		}
	};
}

#endif
