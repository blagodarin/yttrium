// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string_view>

namespace seir
{
	class Blob;
}

namespace Yt
{
	class MeshData;

	MeshData load_obj_mesh(const seir::Blob&, std::string_view source_name);
}
