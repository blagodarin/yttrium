#ifndef _src_gui_key_lookup_h_
#define _src_gui_key_lookup_h_

#include <yttrium/key.h>
#include <yttrium/std/string_view.h>

namespace Yttrium
{
	Key lookup_key(std::string_view name);
}

#endif
