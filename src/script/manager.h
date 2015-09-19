#ifndef _src_script_manager_h_
#define _src_script_manager_h_

namespace Yttrium
{
	class Allocator;

	// NOTE: It should be const, but unfortunately its constness leads to sigsegv.
	extern Allocator* script_manager_allocator;
}

#endif
