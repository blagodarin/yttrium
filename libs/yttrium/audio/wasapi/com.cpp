#include "com.h"

#include <yttrium/exceptions.h>
#include "../../utils/memory.h"

#include <charconv>

#include <comdef.h>

namespace Yttrium
{
	ComInitializer::ComInitializer(DWORD coinit)
	{
		const auto hr = ::CoInitializeEx(nullptr, coinit);
		if (FAILED(hr))
			throw InitializationError{"CoInitializeEx failed: ", hresult_to_string(hr)};
	}

	std::string hresult_to_string(HRESULT hr)
	{
		char* buffer = nullptr;
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<char*>(&buffer), 0, nullptr);
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
			return std::string{buffer, size};
		}
		else
		{
			std::string result{"Error 0xHHHHHHHH."};
			auto* const begin = result.data() + result.find('H');
			std::to_chars(begin, begin + 8, hr, 16);
			return result;
		}
	}
}
