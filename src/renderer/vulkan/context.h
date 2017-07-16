#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class WindowBackend;

	struct VK_Buffer
	{
		const VkDevice _device;
		VkBuffer _buffer = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;

		VK_Buffer(VkDevice device) noexcept : _device{device} {}
		~VK_Buffer() noexcept;

		void create(uint32_t size, VkBufferUsageFlags usage);
		VkMemoryRequirements memory_requirements() const noexcept;
		void bind_memory(VkDeviceMemory);
		void write(const void*, size_t);
	};

	struct VK_DepthBuffer
	{
		const VkDevice _device;
		const VkFormat _format;
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;

		VK_DepthBuffer(VkDevice device, VkFormat format) noexcept : _device{device}, _format{format} {}
		~VK_DepthBuffer() noexcept;

		void create_image(uint32_t width, uint32_t height, VkImageTiling);
		VkMemoryRequirements memory_requirements() const noexcept;
		void bind_memory(VkDeviceMemory);
		void create_view();
	};

	class VulkanContext
	{
	public:
		VulkanContext() = default;
		VulkanContext(const VulkanContext&) = delete;
		~VulkanContext() { reset(); }
		VulkanContext& operator=(const VulkanContext&) = delete;

		void initialize(const WindowBackend&);
		void reset() noexcept;
		void update_uniforms(const void* data, size_t size) { _uniform_buffer->write(data, size); }

	private:
		VkDeviceMemory allocate_memory(const VkMemoryRequirements&, uint32_t flags) const;
		uint32_t find_memory_type(uint32_t type_bits, uint32_t property_bits) const;

	private:
		VkInstance _instance = VK_NULL_HANDLE;
		VkSurfaceKHR _surface = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		uint32_t _queue_family_index = 0;
		VkPhysicalDeviceMemoryProperties _gpu_memory_props = {};
		VkDevice _device = VK_NULL_HANDLE;
		VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
		std::vector<VkImageView> _swapchain_views;
		std::unique_ptr<VK_DepthBuffer> _depth_buffer;
		std::unique_ptr<VK_Buffer> _uniform_buffer;
		VkDescriptorSetLayout _descriptor_set_layout = VK_NULL_HANDLE;
		VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
		VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
		VkDescriptorSet _descriptor_set = VK_NULL_HANDLE;
		VkRenderPass _render_pass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> _framebuffers;
		std::unique_ptr<VK_Buffer> _vertex_buffer;
		VkShaderModule _vertex_shader = VK_NULL_HANDLE;
		VkShaderModule _fragment_shader = VK_NULL_HANDLE;
		VkCommandPool _command_pool = VK_NULL_HANDLE;
		VkCommandBuffer _command_buffer = VK_NULL_HANDLE;
	};
}

#endif
