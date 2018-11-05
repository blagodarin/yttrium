#ifndef _src_gui_key_lookup_h_
#define _src_gui_key_lookup_h_

#include <yttrium/key.h>

#include <string_view>

namespace Yttrium
{
	Y_ENGINE_API Key lookup_key(std::string_view name);
}

#endif
