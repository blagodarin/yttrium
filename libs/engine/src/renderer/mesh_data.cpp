//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include "mesh_data.h"

#include <algorithm>
#include <limits>

namespace Yttrium
{
	bool MeshData::make_uint16_indices(Buffer& buffer) const
	{
		if (_indices.end() != std::find_if(_indices.begin(), _indices.end(), [](auto index) { return index > std::numeric_limits<uint16_t>::max(); }))
			return false;
		buffer.reset(_indices.size() * sizeof(uint16_t));
		auto data = static_cast<uint16_t*>(buffer.data());
		for (const auto index : _indices)
			*data++ = static_cast<uint16_t>(index);
		return true;
	}
}
