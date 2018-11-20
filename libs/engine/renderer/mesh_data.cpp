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
