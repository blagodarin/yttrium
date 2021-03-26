// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/test/utils.h>

#include <yttrium/base/buffer.h>

#include <algorithm>
#include <cstring>
#include <random>

Yt::Buffer make_buffer(std::string_view text)
{
	return { text.size(), text.data() };
}

Yt::Buffer make_random_buffer(size_t size)
{
	std::default_random_engine engine;
	std::uniform_int_distribution<unsigned> distribution{ 0, std::numeric_limits<uint8_t>::max() }; // Visual C++ 2017 doesn't allow uint8_t distribution.
	Yt::Buffer buffer{ size };
	std::generate(buffer.begin(), buffer.end(), [&engine, &distribution] { return static_cast<uint8_t>(distribution(engine)); });
	return buffer;
}
