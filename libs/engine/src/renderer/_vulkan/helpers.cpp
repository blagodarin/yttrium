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

#include "helpers.h"

namespace Yttrium
{
	VulkanVertexFormat::VulkanVertexFormat(const std::vector<VA>& vas)
	{
		_binding.binding = 0;
		_binding.stride = 0;
		_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		_attributes.reserve(vas.size());
		for (uint32_t i = 0; i < vas.size(); ++i)
		{
			auto& attribute = _attributes.emplace_back();
			attribute.location = i;
			attribute.binding = 0;
			attribute.offset = _binding.stride;
			switch (vas[i])
			{
			case VA::f:
				attribute.format = VK_FORMAT_R32_SFLOAT;
				_binding.stride += uint32_t{ sizeof(float) };
				break;
			case VA::f2:
				attribute.format = VK_FORMAT_R32G32_SFLOAT;
				_binding.stride += uint32_t{ sizeof(float) * 2 };
				break;
			case VA::f3:
				attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
				_binding.stride += uint32_t{ sizeof(float) * 3 };
				break;
			case VA::f4:
				attribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				_binding.stride += uint32_t{ sizeof(float) * 4 };
				break;
			}
		}

		_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		_input.pNext = nullptr;
		_input.flags = 0;
		_input.vertexBindingDescriptionCount = 1;
		_input.pVertexBindingDescriptions = &_binding;
		_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributes.size());
		_input.pVertexAttributeDescriptions = _attributes.data();

		_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		_assembly.pNext = nullptr;
		_assembly.flags = 0;
		_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		_assembly.primitiveRestartEnable = VK_FALSE;
	}
}
