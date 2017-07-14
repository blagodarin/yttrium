#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <vector>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class WindowBackend;

	class VulkanContext
	{
	public:
		VulkanContext() = default;
		VulkanContext(const VulkanContext&) = delete;
		~VulkanContext() { reset(); }
		VulkanContext& operator=(const VulkanContext&) = delete;

		void initialize(const WindowBackend&);
		void reset() noexcept;
		void update_uniforms(const void*, size_t size);

	private:
		VkDeviceMemory allocate_memory(const VkMemoryRequirements&, uint32_t flags) const;
		uint32_t find_memory_type(uint32_t type_bits, uint32_t property_bits) const;

	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkPhysicalDeviceMemoryProperties _gpu_memory_props = {};
		VkDevice _device = VK_NULL_HANDLE;
		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
		std::vector<VkImageView> _image_views;
		VkImage _depth_image = VK_NULL_HANDLE;
		VkDeviceMemory _depth_memory = VK_NULL_HANDLE;
		VkImageView _depth_image_view = VK_NULL_HANDLE;
		VkBuffer _uniform_buffer = VK_NULL_HANDLE;
		VkDeviceMemory _uniform_buffer_memory = VK_NULL_HANDLE;
		VkDescriptorSetLayout _descriptor_set_layout = VK_NULL_HANDLE;
		VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
		VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
		VkDescriptorSet _descriptor_set = VK_NULL_HANDLE;
		VkRenderPass _render_pass = VK_NULL_HANDLE;
		VkCommandPool _command_pool = VK_NULL_HANDLE;
		VkCommandBuffer _command_buffer = VK_NULL_HANDLE;
	};
}

#endif
