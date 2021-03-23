// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <comip.h>

namespace Yt
{
	class ComInitializer
	{
	public:
		explicit ComInitializer(DWORD coinit);
		~ComInitializer() noexcept { ::CoUninitialize(); }

		ComInitializer(const ComInitializer&) = delete;
		ComInitializer& operator=(const ComInitializer&) = delete;
	};

	template <typename T>
	using ComPtr = _com_ptr_t<_com_IIID<T, &__uuidof(T)>>;
}
