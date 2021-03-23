// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace Yt
{
	class MeshData;
	class Source;

	MeshData load_obj_mesh(const Source&, std::string_view source_name);
}
