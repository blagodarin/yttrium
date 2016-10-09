#ifndef _src_io_file_h_
#define _src_io_file_h_

#include <yttrium/io/file.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class FilePrivate : public Shared
	{
	public:
		FilePrivate(String&& name, unsigned mode, uint64_t size);

		virtual bool flush() = 0;
		virtual bool resize(uint64_t) = 0;
		virtual size_t read(void*, size_t) = 0;
		virtual size_t read(void*, size_t, uint64_t) const = 0;
		virtual size_t write(const void*, size_t) = 0;
		virtual size_t write(const void*, size_t, uint64_t) = 0;

		static UniquePtr<FilePrivate> open(const StaticString& path, unsigned mode, Allocator&);
		static UniquePtr<FilePrivate> open(File::Special, Allocator&);

	protected:
		const String _name;

	private:
		const unsigned _mode;
		uint64_t _size = 0;
		uint64_t _offset = 0;

		friend File;
		friend Reader;
		friend Writer;
	};
}

#endif
