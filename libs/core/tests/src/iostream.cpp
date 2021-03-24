// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "iostream.h"

#include <yttrium/memory/buffer.h>

#include <sstream>

#include <doctest.h>

namespace Yt
{
	std::ostream& operator<<(std::ostream& stream, const Buffer& buffer)
	{
		return stream << "Buffer(" << buffer.size() << ")";
	}
}

TEST_CASE("iostream.buffer")
{
	{
		std::ostringstream stream;
		stream << Yt::Buffer{};
		CHECK(stream.str() == "Buffer(0)");
	}
	{
		std::ostringstream stream;
		stream << Yt::Buffer{ 1024 };
		CHECK(stream.str() == "Buffer(1024)");
	}
}
