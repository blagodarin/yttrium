#ifndef __SCRIPT_MANAGER_H
#define __SCRIPT_MANAGER_H

namespace Yttrium
{
	class Allocator;

	// NOTE: It should be const, but unfortunately its constness leads to sigsegv.
	extern Allocator* script_manager_allocator;
}

#endif
