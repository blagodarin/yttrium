// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	class Buffer;
	class Source;

	std::unique_ptr<Source> create_source(const std::shared_ptr<const Buffer>&);
}
