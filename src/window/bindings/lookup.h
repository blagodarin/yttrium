#ifndef __WINDOW_BINDINGS_LOOKUP_H
#define __WINDOW_BINDINGS_LOOKUP_H

#include <yttrium/key.h>

namespace Yttrium
{
	class StaticString;

	Key lookup_key(const StaticString& name);
}

#endif // __WINDOW_BINDINGS_LOOKUP_H
