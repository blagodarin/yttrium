#ifndef _src_io_package_h_
#define _src_io_package_h_

#include <yttrium/io/package.h>
#include <yttrium/string.h>

#include <utility>

namespace Yttrium
{
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
