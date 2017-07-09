#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class WindowBackend;

	class VulkanContext
	{
	public:
		VulkanContext() = default;
		VulkanContext(const VulkanContext&) = delete;
		~VulkanContext();
		VulkanContext& operator=(const VulkanContext&) = delete;

		void initialize(const WindowBackend&);

	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
	};
}

#endif
