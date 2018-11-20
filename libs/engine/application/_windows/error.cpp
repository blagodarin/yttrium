#include "error.h"

#include <yttrium/utils/string.h>
#include "../../../core/utils/memory.h"

#include <windows.h>

namespace Yttrium
{
	std::string error_to_string(std::uint32_t code)
	{
		char* buffer = nullptr;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<char*>(&buffer), 0, nullptr);
		if (buffer)
		{
			auto size = std::strlen(buffer);
			if (size > 0 && buffer[size - 1] == '\n')
			{
				--size;
				if (size > 0 && buffer[size - 1] == '\r')
					--size;
			}
			UniquePtr<char[], ::LocalFree> buffer_ptr{ buffer };
			return make_string("Error 0x", Hex32{ code }, ": ", std::string_view{ buffer, size });
		}
		else
			return make_string("Error 0x", Hex32{ code }, '.');
	}
}
