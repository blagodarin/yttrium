// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Yt
{
	template <typename I, typename P>
	constexpr I forward_find_if(I iterator, P&& predicate)
	{
		for (; !predicate(*iterator); ++iterator)
			;
		return iterator;
	}
}
