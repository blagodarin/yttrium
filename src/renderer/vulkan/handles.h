#ifndef _src_renderer_vulkan_handles_h_
#define _src_renderer_vulkan_handles_h_

#include <string>
#include <utility>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	template <typename T>
	class VK_Handle
	{
	public:
		VK_Handle(const VK_Handle&) = delete;
		VK_Handle& operator=(const VK_Handle&) = delete;

		auto get() const noexcept { return _handle; }
		auto release() noexcept { return std::exchange(_handle, T{VK_NULL_HANDLE}); }

		explicit operator bool() const noexcept { return VK_NULL_HANDLE != _handle; }

	protected:
		T _handle = VK_NULL_HANDLE;

		VK_Handle() noexcept = default;
		VK_Handle(T handle) noexcept : _handle{handle} {}
	};

	class VK_HBuffer : public VK_Handle<VkBuffer>
	{
	public:
		explicit VK_HBuffer(VkDevice device) noexcept : _device{device} {}
		~VK_HBuffer() noexcept { if (*this) vkDestroyBuffer(_device, _handle, nullptr); }

		void bind_memory(VkDeviceMemory) const;
		void create(const VkBufferCreateInfo&);
		VkMemoryRequirements memory_requirements() const noexcept;

	private:
		const VkDevice _device;
	};

	class VK_HDeviceMemory : public VK_Handle<VkDeviceMemory>
	{
	public:
		VK_HDeviceMemory() noexcept = default;
		explicit VK_HDeviceMemory(VkDevice device) noexcept : _device{device} {}
		VK_HDeviceMemory(VK_HDeviceMemory&& memory) noexcept : VK_Handle{memory._handle}, _device{memory._device} { memory._handle = VK_NULL_HANDLE; }
		~VK_HDeviceMemory() noexcept { reset(); }
		VK_HDeviceMemory& operator=(VK_HDeviceMemory&&) noexcept;

		void allocate(const VkMemoryAllocateInfo&);
		void reset() noexcept;

	private:
		VkDevice _device = VK_NULL_HANDLE;
	};

	class VK_HImage : public VK_Handle<VkImage>
	{
	public:
		explicit VK_HImage(VkDevice device) noexcept : _device{device} {}
		VK_HImage(VK_HImage&& image) noexcept : VK_Handle{image._handle}, _device{image._device} { image._handle = VK_NULL_HANDLE; }
		~VK_HImage() noexcept { reset(); }
		VK_HImage& operator=(VK_HImage&&) noexcept;

		void bind_memory(VkDeviceMemory) const;
		void create(const VkImageCreateInfo&);
		VkMemoryRequirements memory_requirements() const noexcept;
		void reset() noexcept;

	private:
		VkDevice _device = VK_NULL_HANDLE;
	};

	class VK_HImageView : public VK_Handle<VkImageView>
	{
	public:
		explicit VK_HImageView(VkDevice device) noexcept : _device{device} {}
		VK_HImageView(VK_HImageView&& view) noexcept : VK_Handle{view._handle}, _device{view._device} { view._handle = VK_NULL_HANDLE; }
		~VK_HImageView() noexcept { if (*this) vkDestroyImageView(_device, _handle, nullptr); }

		void create(const VkImageViewCreateInfo&);

	private:
		const VkDevice _device;
	};

	class VK_HSwapchain : public VK_Handle<VkSwapchainKHR>
	{
	public:
		explicit VK_HSwapchain(VkDevice device) noexcept : _device{device} {}
		~VK_HSwapchain() noexcept { if (*this) vkDestroySwapchainKHR(_device, _handle, nullptr); }

		void create(const VkSwapchainCreateInfoKHR&);

	private:
		const VkDevice _device;
	};

	void vulkan_result_check(VkResult, const std::string& function);
	std::string vulkan_result_to_string(VkResult);
}

#define Y_VK_CHECK(call) if (const auto result = (call)) ::Yttrium::vulkan_result_check(result, #call);

#endif
