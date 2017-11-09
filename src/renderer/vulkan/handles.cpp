#include "handles.h"

#include <cassert>
#include <stdexcept>

namespace Yttrium
{
	void VK_HBuffer::bind_memory(VkDeviceMemory memory) const
	{
		assert(*this && VK_NULL_HANDLE != memory);
		Y_VK_CHECK(vkBindBufferMemory(_device, _handle, memory, 0));
	}

	void VK_HBuffer::create(const VkBufferCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateBuffer(_device, &info, nullptr, &_handle));
	}

	VkMemoryRequirements VK_HBuffer::memory_requirements() const noexcept
	{
		assert(*this);
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(_device, _handle, &requirements);
		return requirements;
	}

	VK_HDeviceMemory& VK_HDeviceMemory::operator=(VK_HDeviceMemory&& memory) noexcept
	{
		reset();
		_handle = memory._handle;
		_device = memory._device;
		memory._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_HDeviceMemory::allocate(const VkMemoryAllocateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkAllocateMemory(_device, &info, nullptr, &_handle));
	}

	void VK_HDeviceMemory::reset() noexcept
	{
		if (*this)
			vkFreeMemory(_device, _handle, nullptr);
	}

	VK_HImage& VK_HImage::operator=(VK_HImage&& image) noexcept
	{
		reset();
		_handle = image._handle;
		_device = image._device;
		image._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_HImage::bind_memory(VkDeviceMemory memory) const
	{
		assert(*this && VK_NULL_HANDLE != memory);
		Y_VK_CHECK(vkBindImageMemory(_device, _handle, memory, 0));
	}

	void VK_HImage::create(const VkImageCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateImage(_device, &info, nullptr, &_handle));
	}

	VkMemoryRequirements VK_HImage::memory_requirements() const noexcept
	{
		assert(*this);
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(_device, _handle, &requirements);
		return requirements;
	}

	void VK_HImage::reset() noexcept
	{
		if (*this)
			vkDestroyImage(_device, _handle, nullptr);
	}

	VK_HImageView& VK_HImageView::operator=(VK_HImageView&& view) noexcept
	{
		reset();
		_handle = view._handle;
		_device = view._device;
		view._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_HImageView::create(const VkImageViewCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateImageView(_device, &info, nullptr, &_handle));
	}

	void VK_HImageView::reset() noexcept
	{
		if (*this)
			vkDestroyImageView(_device, _handle, nullptr);
	}

	void VK_HInstance::create(const VkInstanceCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateInstance(&info, nullptr, &_handle));
	}

	std::vector<VkPhysicalDevice> VK_HInstance::physical_devices() const
	{
		assert(*this);
		uint32_t count = 0;
		Y_VK_CHECK(vkEnumeratePhysicalDevices(_handle, &count, nullptr));
		std::vector<VkPhysicalDevice> devices(count);
		Y_VK_CHECK(vkEnumeratePhysicalDevices(_handle, &count, devices.data()));
		return devices;
	}

	void VK_HSampler::create(const VkSamplerCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateSampler(_device, &info, nullptr, &_handle));
	}

	void VK_HSwapchain::create(const VkSwapchainCreateInfoKHR& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateSwapchainKHR(_device, &info, nullptr, &_handle));
	}

	void vulkan_result_check(VkResult result, const std::string& function)
	{
		throw std::runtime_error{function.substr(0, function.find('(')) + " = " + vulkan_result_to_string(result)};
	}

	std::string vulkan_result_to_string(VkResult result)
	{
		switch (result)
		{
		case VK_SUCCESS: return "VK_SUCCESS";
		case VK_NOT_READY: return "VK_NOT_READY";
		case VK_TIMEOUT: return "VK_TIMEOUT";
		case VK_EVENT_SET: return "VK_EVENT_SET";
		case VK_EVENT_RESET: return "VK_EVENT_RESET";
		case VK_INCOMPLETE: return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
#if VK_HEADER_VERSION < 54
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHX: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHX";
#else
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
#endif
		default: return std::to_string(result);
		}
	}
}
