#ifndef _src_io_package_h_
#define _src_io_package_h_

#include <yttrium/io/package.h>

#include "file.h"

#include <utility>

namespace Yttrium
{
	class PackedFile : public FilePrivate
	{
	public:
		PackedFile(String&& name, unsigned mode, uint64_t size, const SharedPtr<FilePrivate>& package, uint64_t base)
			: FilePrivate(std::move(name), mode, size)
			, _package(package)
			, _base(base)
		{
		}

		bool flush() override;
		bool resize(uint64_t size) override;
		size_t read(void*, size_t) override;
		size_t read(void*, size_t, uint64_t) const override;
		size_t write(const void*, size_t) override;
		size_t write(const void*, size_t, uint64_t) override;

	private:
		const SharedPtr<FilePrivate> _package;
		const uint64_t _base;
	};

	class BadPackage
	{
	public:
		BadPackage(String&& what) : _what(std::move(what)) {}
		StaticString what() const { return _what; }
	private:
		const String _what;
	};
}

#endif
