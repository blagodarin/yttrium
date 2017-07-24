#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class WindowBackend;

	struct VK_Instance
	{
		VkInstance _handle = VK_NULL_HANDLE;

		VK_Instance();
		~VK_Instance() noexcept;
	};

	struct VK_Surface
	{
		const VK_Instance& _instance;
		VkSurfaceKHR _handle = VK_NULL_HANDLE;

		VK_Surface(const VK_Instance&, const WindowBackend&);
		~VK_Surface() noexcept;
	};

	struct VK_PhysicalDevice
	{
		const VK_Surface& _surface;
		VkPhysicalDevice _handle = VK_NULL_HANDLE;
		uint32_t _queue_family_index = 0;
		VkSurfaceCapabilitiesKHR _surface_capabilities;
		VkPhysicalDeviceMemoryProperties _memory_properties;

		VK_PhysicalDevice(const VK_Surface&);
		~VK_PhysicalDevice() noexcept = default;

		VkCompositeAlphaFlagBitsKHR composite_alpha() const noexcept;
		uint32_t memory_type_index(uint32_t type_bits, VkFlags) const;
		std::vector<VkSurfaceFormatKHR> surface_formats() const;
		VkSurfaceTransformFlagBitsKHR surface_transform() const noexcept;
		VkImageTiling tiling(VkFormat, VkFlags) const;
	};

	struct VK_Device
	{
		const VK_PhysicalDevice& _physical_device;
		VkDevice _handle = VK_NULL_HANDLE;
		VkQueue _graphics_queue = VK_NULL_HANDLE;
		VkQueue _present_queue = VK_NULL_HANDLE;

		VK_Device(const VK_PhysicalDevice&);
		~VK_Device() noexcept;

		VkDeviceMemory allocate_memory(const VkMemoryRequirements&, VkFlags) const;
	};

	struct VK_Swapchain
	{
		const VK_Device& _device;
		VkFormat _format = VK_FORMAT_UNDEFINED;
		VkSwapchainKHR _handle = VK_NULL_HANDLE;
		std::vector<VkImageView> _views;

		VK_Swapchain(const VK_Device& device) noexcept : _device{device} {}
		~VK_Swapchain() noexcept;

		void create();
		void create_views();
		void present(uint32_t index, VkSemaphore) const;

		VkAttachmentDescription attachment_description() const noexcept;
	};

	struct VK_DepthBuffer
	{
		const VK_Device& _device;
		VkFormat _format = VK_FORMAT_UNDEFINED;
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;

		VK_DepthBuffer(const VK_Device& device) noexcept : _device{device} {}
		~VK_DepthBuffer() noexcept;

		void create_image(VkFormat);
		void allocate_memory(VkFlags);
		void create_view();

		VkAttachmentDescription attachment_description() const noexcept;
	};

	struct VK_RenderPass
	{
		const VK_Device& _device;
		VkRenderPass _handle = VK_NULL_HANDLE;

		VK_RenderPass(const VK_Device& device) noexcept : _device{device} {}
		~VK_RenderPass() noexcept;

		void create(const VK_Swapchain&, const VK_DepthBuffer&);
	};

	struct VK_Framebuffers
	{
		const VK_Device& _device;
		std::vector<VkFramebuffer> _handles;

		VK_Framebuffers(const VK_Device& device) noexcept : _device{device} {}
		~VK_Framebuffers() noexcept;

		void create(const VK_RenderPass&, const VK_Swapchain&, const VK_DepthBuffer&);
	};

	struct VK_Buffer
	{
		const VK_Device& _device;
		const uint32_t _size;
		VkBuffer _handle = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;

		VK_Buffer(const VK_Device& device, uint32_t size) noexcept : _device{device}, _size{size} {}
		~VK_Buffer() noexcept;

		void create(VkBufferUsageFlags usage);
		void allocate_memory(VkFlags);

		void write(const void*, size_t);
	};

	struct VK_Semaphore
	{
		const VK_Device& _device;
		VkSemaphore _handle = VK_NULL_HANDLE;

		VK_Semaphore(const VK_Device& device) noexcept : _device{device} {}
		~VK_Semaphore() noexcept;

		void create();
	};

	class VulkanContext
	{
	public:
		VulkanContext(const WindowBackend&);
		~VulkanContext() noexcept;

		void render();
		void update_uniforms(const void* data, size_t size) { _uniform_buffer.write(data, size); }

	private:
		VK_Instance _instance;
		VK_Surface _surface;
		VK_PhysicalDevice _physical_device;
		VK_Device _device;
		VK_Swapchain _swapchain;
		VK_DepthBuffer _depth_buffer;
		VK_RenderPass _render_pass;
		VK_Framebuffers _framebuffers;
		VK_Buffer _uniform_buffer;
		VK_Buffer _vertex_buffer;
		VK_Semaphore _image_acquired;
		VK_Semaphore _rendering_complete;
		VkDescriptorSetLayout _descriptor_set_layout = VK_NULL_HANDLE;
		VkPipelineLayout _pipeline_layout = VK_NULL_HANDLE;
		VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
		VkDescriptorSet _descriptor_set = VK_NULL_HANDLE;
		VkShaderModule _vertex_shader = VK_NULL_HANDLE;
		VkShaderModule _fragment_shader = VK_NULL_HANDLE;
		VkCommandPool _command_pool = VK_NULL_HANDLE;
		VkCommandBuffer _command_buffer = VK_NULL_HANDLE;
		VkPipeline _pipeline = VK_NULL_HANDLE;
	};
}

#endif
