#ifndef _src_io_posix_file_h_
#define _src_io_posix_file_h_

#include <yttrium/io/file.h>

#include <string>

namespace Yttrium
{
	class FilePrivate
	{
	public:
		FilePrivate(std::string&& name, unsigned mode, uint64_t size, int descriptor);
		~FilePrivate();

		bool flush();
		size_t read(void*, size_t);
		size_t read(void*, size_t, uint64_t) const;
		bool resize(uint64_t);
		uint64_t size() const { return _size; }
		size_t write(const void*, size_t);
		size_t write(const void*, size_t, uint64_t);

		static std::shared_ptr<FilePrivate> open(const StaticString& path, unsigned mode);
		static std::shared_ptr<FilePrivate> open(File::Special);

	private:
		const std::string _name; // TODO: Don't store names in Files.
		const unsigned _mode;
		uint64_t _size = 0;
		uint64_t _offset = 0;
		const int _descriptor;
		bool _auto_close = false;
		bool _auto_remove = false;

		friend File;
		friend Writer;
	};
}

#endif
