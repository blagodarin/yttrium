#ifndef _src_renderer_vulkan_helpers_h_
#define _src_renderer_vulkan_helpers_h_

#include "../mesh_data.h"

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class VulkanVertexFormat
	{
	public:
		std::vector<VkVertexInputAttributeDescription> _attributes;
		VkVertexInputBindingDescription _binding;
		VkPipelineVertexInputStateCreateInfo _input;
		VkPipelineInputAssemblyStateCreateInfo _assembly;

		explicit VulkanVertexFormat(const std::vector<VA>&);

		VulkanVertexFormat(const VulkanVertexFormat&) = delete;
		VulkanVertexFormat& operator=(const VulkanVertexFormat&) = delete;
	};
}

#endif
