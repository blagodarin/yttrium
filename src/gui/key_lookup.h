#ifndef _src_gui_key_lookup_h_
#define _src_gui_key_lookup_h_

#include <yttrium/key.h>

namespace Yttrium
{
	class StaticString;

	Key lookup_key(const StaticString& name);
}

#endif
