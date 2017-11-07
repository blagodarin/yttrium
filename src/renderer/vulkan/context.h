#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <functional>
#include <memory>

#include "wrappers.h"

namespace Yttrium
{
	class VK_HDeviceMemory;
	class VK_HImage;
	class VK_HImageView;
	class VK_HSampler;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(const VK_Device&, const VK_CommandPool&, const VK_PipelineLayout&, const std::vector<VkPipelineShaderStageCreateInfo>&);
		~VulkanSwapchain() noexcept = default;

		void render(const std::function<void(VkCommandBuffer, const std::function<void(const std::function<void()>&)>&)>&) const; // TODO: Improve readability.

	private:
		void present(uint32_t framebuffer_index, VkSemaphore) const;

	public:
		const VK_Device& _device;
		VK_Swapchain _swapchain;
		VK_DepthBuffer _depth_buffer;
		VK_RenderPass _render_pass;
		VK_Framebuffers _framebuffers;
		VK_Pipeline _pipeline;
		VK_Semaphore _image_acquired;
		VK_Semaphore _rendering_complete;
		VK_CommandBuffer _command_buffer;
	};

	class VulkanContext
	{
	public:
		explicit VulkanContext(const WindowBackend&);

		VK_CommandBuffer allocate_command_buffer() const { return VK_CommandBuffer{_command_pool}; }
		VK_HDeviceMemory allocate_memory(const VkMemoryRequirements&, VkMemoryPropertyFlags) const;
		const VK_CommandPool& command_pool() const noexcept { return _command_pool; }
		VK_HImage create_texture_2d_image(size_t width, size_t height, VkFormat);
		VK_HSampler create_texture_2d_sampler();
		VK_HImageView create_texture_2d_view(VkImage, VkFormat);
		const VK_Device& device() const noexcept { return _device; }

	private:
		const VK_Instance _instance;
		const VK_Surface _surface;
		const VK_PhysicalDevice _physical_device;
		const VK_Device _device;
		const VK_CommandPool _command_pool;
	};
}

#endif
