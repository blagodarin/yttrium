#ifndef _src_storage_posix_file_h_
#define _src_storage_posix_file_h_

#include <yttrium/storage/file.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class FilePrivate
	{
	public:
		FilePrivate(String&& name, unsigned mode, uint64_t size, int descriptor);
		~FilePrivate();

		bool flush();
		const String& name() const { return _name; }
		size_t read(void*, size_t);
		size_t read_at(uint64_t, void*, size_t) const;
		bool resize(uint64_t);
		uint64_t size() const { return _size; }
		void unlink() { _auto_remove = true; }
		size_t write(const void*, size_t);
		size_t write_at(uint64_t, const void*, size_t);

		static std::shared_ptr<FilePrivate> open(const StaticString& path, unsigned mode);

	private:
		const String _name;
		const unsigned _mode;
		uint64_t _size = 0;
		uint64_t _offset = 0;
		const int _descriptor;
		bool _auto_remove = false;

		friend File;
		friend Writer;
	};
}

#endif
