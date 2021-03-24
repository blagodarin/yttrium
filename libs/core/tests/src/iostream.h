// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iosfwd>

namespace Yt
{
	class Buffer;

	std::ostream& operator<<(std::ostream&, const Buffer&);
}
