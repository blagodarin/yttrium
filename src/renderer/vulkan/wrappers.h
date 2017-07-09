#ifndef _src_renderer_vulkan_wrappers_h_
#define _src_renderer_vulkan_wrappers_h_

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class WindowBackend;

	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance();

		VkInstance get() const noexcept { return _instance; }

		VulkanInstance(const VulkanInstance&) = delete;
		VulkanInstance& operator=(const VulkanInstance&) = delete;

	private:
		VkInstance _instance = VK_NULL_HANDLE;
	};

	class VulkanSurface
	{
	public:
		VulkanSurface(VkInstance, const WindowBackend&);
		~VulkanSurface();

	private:
		const VkInstance _instance;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
	};
}

#endif
