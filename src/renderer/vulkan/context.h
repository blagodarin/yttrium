#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <functional>
#include <memory>

#include "wrappers.h"

namespace Yttrium
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(const VK_Device&, const VK_CommandPool&, const VK_PipelineLayout&, VkShaderModule vertex_shader, VkShaderModule fragment_shader);
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

		void render();
		void update_uniforms(const void* data, size_t size) { _uniform_buffer.write(data, size); }

	private:
		const VK_Instance _instance;
		const VK_Surface _surface;
		const VK_PhysicalDevice _physical_device;
		const VK_Device _device;
		VK_Buffer _uniform_buffer;
		VK_Buffer _vertex_buffer;
		VK_CommandPool _command_pool;
		VK_DescriptorSetLayout _descriptor_set_layout;
		VK_DescriptorPool _descriptor_pool;
		VK_DescriptorSet _descriptor_set;
		VK_PipelineLayout _pipeline_layout;
		VK_ShaderModule _vertex_shader;
		VK_ShaderModule _fragment_shader;
		std::unique_ptr<VulkanSwapchain> _swapchain;
	};
}

#endif
