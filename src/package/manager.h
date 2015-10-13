#ifndef _src_package_manager_h_
#define _src_package_manager_h_

namespace Yttrium
{
	class Allocator;
	class File;
	class StaticString;

	File open_file_for_reading(const StaticString& name, Allocator& allocator);
}

#endif
