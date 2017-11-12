#ifndef _src_renderer_vulkan_swapchain_h_
#define _src_renderer_vulkan_swapchain_h_

#include <functional>

#include "depth_buffer.h"
#include "wrappers.h"

namespace Yttrium
{
	class VulkanContext;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(const VulkanContext&, const VK_PipelineLayout&, const std::vector<VkPipelineShaderStageCreateInfo>&);
		~VulkanSwapchain() noexcept = default;

		void render(const std::function<void(VkCommandBuffer, const std::function<void(const std::function<void()>&)>&)>&) const; // TODO: Improve readability.

	private:
		void present(uint32_t framebuffer_index, VkSemaphore) const;

	public:
		const VulkanContext& _context;
		VK_Swapchain _swapchain;
		VulkanDepthBuffer _depth_buffer;
		VK_RenderPass _render_pass;
		VK_Framebuffers _framebuffers;
		VK_Pipeline _pipeline;
		VK_Semaphore _image_acquired;
		VK_Semaphore _rendering_complete;
		VK_CommandBuffer _command_buffer;
	};
}

#endif
