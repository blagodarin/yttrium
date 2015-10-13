#ifndef _src_package_package_h_
#define _src_package_package_h_

#include <yttrium/package.h>

#include <yttrium/file.h>
#include <yttrium/string.h>

namespace Yttrium
{
	struct PackedFile
	{
		File*    file;
		uint64_t size;

		PackedFile(File* file = nullptr, uint64_t size = 0)
			: file(file)
			, size(size)
		{
		}
	};

	class BadPackage
	{
	public:
		BadPackage(String&& what) : _what(std::move(what)) {}
		StaticString what() const { return _what; }
	private:
		const String _what;
	};

	class PackageReaderImpl : public PackageReader
	{
	public:

		PackageReaderImpl(File&& file, Allocator& allocator) : _allocator(allocator), _file(std::move(file)) {}

		File open_file(const StaticString& name) override;

	protected:

		virtual PackedFile do_open_file(const StaticString& name) = 0;

	protected:

		Allocator& _allocator;
		File _file;
	};

	class PackageWriterImpl : public PackageWriter
	{
	public:

		PackageWriterImpl(File&& file, Allocator& allocator) : _allocator(allocator), _file(std::move(file)) {}

		File open_file(const StaticString& name) override;

	protected:

		virtual PackedFile do_open_file(const StaticString& name) = 0;

	protected:

		Allocator& _allocator;
		File _file;
	};
}

#endif
