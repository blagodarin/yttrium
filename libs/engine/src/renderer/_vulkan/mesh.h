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

#pragma once

#include <yttrium/renderer/mesh.h>
#include "buffer.h"

namespace Yttrium
{
	struct VulkanMesh final : public Mesh
	{
		VulkanBuffer _vertex_buffer;
		VulkanBuffer _index_buffer;
		const VkIndexType _index_type;
		const uint32_t _index_count;

		VulkanMesh(const VulkanContext&, size_t vertex_buffer_size, size_t index_buffer_size, VkIndexType, size_t index_count);

		void draw(VkCommandBuffer) const noexcept;
	};
}
