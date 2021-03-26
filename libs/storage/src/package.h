// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/base/exceptions.h>

namespace Yt
{
	class BadPackage : public DataError
	{
	public:
		template <typename... Args>
		explicit BadPackage(Args&&... args)
			: DataError(std::forward<Args>(args)...)
		{}
	};
}
