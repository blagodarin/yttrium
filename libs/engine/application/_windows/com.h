#pragma once

#include <comip.h>

namespace Yttrium
{
	class ComInitializer
	{
	public:
		ComInitializer(DWORD coinit);
		~ComInitializer() noexcept { ::CoUninitialize(); }

		ComInitializer(const ComInitializer&) = delete;
		ComInitializer& operator=(const ComInitializer&) = delete;
	};

	template <typename T>
	using ComPtr = _com_ptr_t<_com_IIID<T, &__uuidof(T)>>;
}
