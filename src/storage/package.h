#ifndef _src_storage_package_h_
#define _src_storage_package_h_

#include <yttrium/storage/package.h>
#include <yttrium/string.h>

#include <utility>

namespace Yttrium
{
	class BadPackage // TODO: Use variadic constructor.
	{
	public:
		BadPackage(String&& what) : _what(std::move(what)) {}
		StaticString what() const { return _what; }
	private:
		const String _what;
	};
}

#endif
