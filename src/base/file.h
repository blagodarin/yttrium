#ifndef _src_base_file_h_
#define _src_base_file_h_

#include <yttrium/file.h>
#include <yttrium/string.h>

namespace Yttrium
{
	class FilePrivate : public Shared
	{
	public:
		FilePrivate(String&& name, unsigned mode, uint64_t size);

		const String& name() const { return _name; }
		void update_size(uint64_t offset); // TODO: Remove.

		virtual bool flush() = 0;
		virtual bool resize(uint64_t) = 0;
		virtual size_t read(void*, size_t) = 0;
		virtual size_t read(void*, size_t, uint64_t) = 0;
		virtual size_t write(const void*, size_t) = 0;
		virtual size_t write(const void*, size_t, uint64_t) = 0;

		static UniquePtr<FilePrivate> open(const StaticString& path, unsigned mode, Allocator&);
		static UniquePtr<FilePrivate> open(File::Special, Allocator&);

		// Package API.
		static SharedPtr<FilePrivate> get(File& file) { return file._private; }
		static void set(File& file, const SharedPtr<FilePrivate>& file_private) { file._private = file_private; }

	private:
		const String _name;
		const unsigned _mode;
		uint64_t _size = 0;
		uint64_t _offset = 0;

		friend File;
	};
}

#endif
