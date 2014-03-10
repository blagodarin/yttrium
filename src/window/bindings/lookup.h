#ifndef __WINDOW_BINDINGS_LOOKUP_H
#define __WINDOW_BINDINGS_LOOKUP_H

#include <yttrium/key.h>
#include <yttrium/static_string.h>

namespace Yttrium
{

Key lookup_key(const StaticString name);

} // namespace Yttrium

#endif // __WINDOW_BINDINGS_LOOKUP_H
