#ifndef __TERMINAL_BINDINGS_LOOKUP_H
#define __TERMINAL_BINDINGS_LOOKUP_H

#include <yttrium/key.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

Key lookup_key(const StaticString name);

} // namespace Yttrium

#endif // __TERMINAL_BINDINGS_LOOKUP_H
