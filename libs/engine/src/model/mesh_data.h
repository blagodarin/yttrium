//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/memory/buffer.h>

#include <vector>

namespace Yt
{
	/// Vertex attribute type.
	enum class VA
	{
		f,  ///< Single float.
		f2, ///< Vector of 2 floats.
		f3, ///< Vector of 3 floats.
		f4, ///< Vector of 4 floats.
	};

	class MeshData
	{
	public:
		std::vector<VA> _vertex_format;
		Buffer _vertex_data;
		std::vector<uint32_t> _indices;

		bool make_uint16_indices(Buffer&) const;
	};
}
