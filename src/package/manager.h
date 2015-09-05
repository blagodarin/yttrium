#ifndef __PACKAGE_MANAGER_H
#define __PACKAGE_MANAGER_H

namespace Yttrium
{
	class Allocator;
	class File;
	class StaticString;

	File open_file_for_reading(const StaticString& name, Allocator* allocator);
}

#endif
