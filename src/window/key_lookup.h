#ifndef _src_window_bindings_lookup_h_
#define _src_window_bindings_lookup_h_

#include <yttrium/key.h>

namespace Yttrium
{
	class StaticString;

	Key lookup_key(const StaticString& name);
}

#endif
