#ifndef _src_renderer_vulkan_mesh_h_
#define _src_renderer_vulkan_mesh_h_

#include <yttrium/renderer/mesh.h>

namespace Yttrium
{
	struct VulkanMesh final : public Mesh
	{
		VK_Buffer _vertex_buffer;
		VK_Buffer _index_buffer;

		VulkanMesh(const VK_Device& device, size_t vertex_buffer_size, size_t index_buffer_size)
			: _vertex_buffer{device, static_cast<uint32_t>(vertex_buffer_size)}
			, _index_buffer{device, static_cast<uint32_t>(index_buffer_size)}
		{
		}
	};
}

#endif
