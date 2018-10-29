#ifndef _libs_yttrium_audio_wasapi_com_h_
#define _libs_yttrium_audio_wasapi_com_h_

#include <string>

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

	std::string hresult_to_string(HRESULT);
}

#endif
