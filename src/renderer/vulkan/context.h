#ifndef _src_renderer_vulkan_context_h_
#define _src_renderer_vulkan_context_h_

#include <functional>
#include <memory>
#include <optional>
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

		std::vector<VkPhysicalDevice> physical_device_handles() const;
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

		explicit VK_PhysicalDevice(const VK_Surface&);
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

		explicit VK_Device(const VK_PhysicalDevice&);
		~VK_Device() noexcept;

		VkDeviceMemory allocate_memory(const VkMemoryRequirements&, VkFlags) const;
		void wait_idle();
	};

	struct VK_Swapchain
	{
		struct OutOfDate : std::runtime_error
		{
			OutOfDate() : std::runtime_error{"VK_ERROR_OUT_OF_DATE_KHR"} {}
		};

		const VK_Device& _device;
		VkFormat _format = VK_FORMAT_UNDEFINED;
		VkSwapchainKHR _handle = VK_NULL_HANDLE;
		std::vector<VkImageView> _views;

		explicit VK_Swapchain(const VK_Device& device) noexcept : _device{device} {}
		~VK_Swapchain() noexcept;

		void create();
		void create_views();

		VkAttachmentDescription attachment_description() const noexcept;

		uint32_t acquire_next_image(VkSemaphore) const;
		void present(uint32_t framebuffer_index, VkSemaphore) const;
	};

	struct VK_DepthBuffer
	{
		const VK_Device& _device;
		VkFormat _format = VK_FORMAT_UNDEFINED;
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;

		explicit VK_DepthBuffer(const VK_Device& device) noexcept : _device{device} {}
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

		explicit VK_RenderPass(const VK_Device& device) noexcept : _device{device} {}
		~VK_RenderPass() noexcept;

		void create(const VK_Swapchain&, const VK_DepthBuffer&);
	};

	struct VK_Framebuffers
	{
		const VK_Device& _device;
		std::vector<VkFramebuffer> _handles;

		explicit VK_Framebuffers(const VK_Device& device) noexcept : _device{device} {}
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

		VkDescriptorBufferInfo descriptor_buffer_info() const noexcept;
		void write(const void*, size_t);
	};

	struct VK_Semaphore
	{
		const VK_Device& _device;
		VkSemaphore _handle = VK_NULL_HANDLE;

		explicit VK_Semaphore(const VK_Device&);
		~VK_Semaphore() noexcept;
	};

	struct VK_DescriptorSetLayout
	{
		const VK_Device& _device;
		VkDescriptorSetLayout _handle = VK_NULL_HANDLE;

		struct Binding : VkDescriptorSetLayoutBinding
		{
			Binding(VkDescriptorType type, VkShaderStageFlags flags) noexcept
			{
				binding = 0;
				descriptorType = type;
				descriptorCount = 1;
				stageFlags = flags;
				pImmutableSamplers = nullptr;
			}
		};

		VK_DescriptorSetLayout(const VK_Device&, std::vector<Binding>&&);
		~VK_DescriptorSetLayout() noexcept;
	};

	struct VK_PipelineLayout
	{
		const VK_Device& _device;
		VkPipelineLayout _handle = VK_NULL_HANDLE;

		VK_PipelineLayout(const VK_Device&, std::initializer_list<VkDescriptorSetLayout>);
		~VK_PipelineLayout() noexcept;
	};

	struct VK_Pipeline
	{
		const VK_Device& _device;
		VkPipeline _handle = VK_NULL_HANDLE;

		explicit VK_Pipeline(const VK_Device& device) noexcept : _device{device} {}
		~VK_Pipeline() noexcept;

		void create(const VK_PipelineLayout&, VkRenderPass, VkShaderModule vertex_shader, VkShaderModule fragment_shader);
	};

	struct VK_CommandPool
	{
		const VK_Device& _device;
		VkCommandPool _handle = VK_NULL_HANDLE;

		VK_CommandPool(const VK_Device&, uint32_t queue_family_index);
		~VK_CommandPool() noexcept;
	};

	struct VK_CommandBuffer
	{
		const VK_CommandPool& _pool;
		VkCommandBuffer _handle = VK_NULL_HANDLE;

		explicit VK_CommandBuffer(const VK_CommandPool&);
		~VK_CommandBuffer() noexcept;

		void begin() const;
		void end() const;
		void submit(VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) const;
	};

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
		~VulkanContext() noexcept;

		void render();
		void update_uniforms(const void* data, size_t size) { _uniform_buffer.write(data, size); }

	private:
		VK_Instance _instance;
		VK_Surface _surface;
		VK_PhysicalDevice _physical_device;
		VK_Device _device;
		VK_Buffer _uniform_buffer;
		VK_Buffer _vertex_buffer;
		VK_CommandPool _command_pool;
		VK_DescriptorSetLayout _descriptor_set_layout;
		VK_PipelineLayout _pipeline_layout;
		VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
		VkDescriptorSet _descriptor_set = VK_NULL_HANDLE;
		VkShaderModule _vertex_shader = VK_NULL_HANDLE;
		VkShaderModule _fragment_shader = VK_NULL_HANDLE;
		std::unique_ptr<VulkanSwapchain> _swapchain;
	};
}

#endif
