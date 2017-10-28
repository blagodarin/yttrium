#include "mesh.h"

namespace Yttrium
{
	VulkanMesh::VulkanMesh(const VK_Device& device, size_t vertex_buffer_size, size_t index_buffer_size, VkIndexType index_type, size_t index_count)
		: _vertex_buffer{device, static_cast<uint32_t>(vertex_buffer_size)}
		, _index_buffer{device, static_cast<uint32_t>(index_buffer_size)}
		, _index_type{index_type}
		, _index_count{static_cast<uint32_t>(index_count)}
	{
	}

	void VulkanMesh::draw(VkCommandBuffer command_buffer) const
	{
		const VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(command_buffer, 0, 1, &_vertex_buffer._handle, &offset);
		vkCmdBindIndexBuffer(command_buffer, _index_buffer._handle, 0, _index_type);
		vkCmdDrawIndexed(command_buffer, _index_count, 1, 0, 0, 0);
	}
}
