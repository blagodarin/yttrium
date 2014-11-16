#ifndef __SCRIPT_MANAGER_H
#define __SCRIPT_MANAGER_H

namespace Yttrium
{

class Allocator;

// NOTE: It should be const, bun unfortunally constness leads to sigsegv.
extern Allocator* script_manager_allocator;

} // namespace Yttrium

#endif // __SCRIPT_MANAGER_H
