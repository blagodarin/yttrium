#include "com.h"

#include <yttrium/exceptions.h>
#include "error.h"

#include <comdef.h>

namespace Yttrium
{
	ComInitializer::ComInitializer(DWORD coinit)
	{
		const auto hr = ::CoInitializeEx(nullptr, coinit);
		if (FAILED(hr))
			throw InitializationError{ "CoInitializeEx failed: ", error_to_string(hr) };
	}
}
