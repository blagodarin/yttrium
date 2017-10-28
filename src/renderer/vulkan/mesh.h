#ifndef _src_renderer_vulkan_mesh_h_
#define _src_renderer_vulkan_mesh_h_

#include <yttrium/renderer/mesh.h>
#include "wrappers.h"

namespace Yttrium
{
	struct VulkanMesh final : public Mesh
	{
		VK_Buffer _vertex_buffer;
		VK_Buffer _index_buffer;
		const VkIndexType _index_type;
		const uint32_t _index_count;

		VulkanMesh(const VK_Device&, size_t vertex_buffer_size, size_t index_buffer_size, VkIndexType, size_t index_count);

		void draw(VkCommandBuffer) const noexcept;
	};
}

#endif
