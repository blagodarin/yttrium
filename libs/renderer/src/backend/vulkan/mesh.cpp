// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "mesh.h"

namespace Yt
{
	VulkanMesh::VulkanMesh(const VulkanContext& context, size_t vertex_buffer_size, size_t index_buffer_size, VkIndexType index_type, size_t index_count)
		: _vertex_buffer{ context, static_cast<uint32_t>(vertex_buffer_size), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
		, _index_buffer{ context, static_cast<uint32_t>(index_buffer_size), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
		, _index_type{ index_type }
		, _index_count{ static_cast<uint32_t>(index_count) }
	{
	}

	void VulkanMesh::draw(VkCommandBuffer command_buffer) const noexcept
	{
		if (command_buffer == VK_NULL_HANDLE) // TODO: Remove.
			return;
		const VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(command_buffer, 0, 1, &_vertex_buffer.get(), &offset);
		vkCmdBindIndexBuffer(command_buffer, _index_buffer.get(), 0, _index_type);
		vkCmdDrawIndexed(command_buffer, _index_count, 1, 0, 0, 0);
	}
}
