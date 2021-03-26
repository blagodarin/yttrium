// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "context.h"

#include <yttrium/application/window.h>
#include <yttrium/base/logger.h>
#include "handles.h"

#include <algorithm>
#include <array>
#include <stdexcept>

#include <fmt/format.h>

namespace
{
	void print_vulkan_layers_available()
	{
		uint32_t count = 0;
		Y_VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));
		std::vector<VkLayerProperties> layers(count);
		Y_VK_CHECK(vkEnumerateInstanceLayerProperties(&count, layers.data()));
		Yt::Logger::write("Vulkan layers available:");
		for (const auto& layer : layers)
			Yt::Logger::write(fmt::format("  {} - {}", layer.layerName, layer.description));
		Yt::Logger::write("");
	}

	VkInstance create_vulkan_instance()
	{
		VkApplicationInfo application_info;
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		application_info.pApplicationName = nullptr;
		application_info.applicationVersion = 0;
		application_info.pEngineName = nullptr;
		application_info.engineVersion = 0;
		application_info.apiVersion = VK_API_VERSION_1_0;

		static const std::array layers{
#ifndef NDEBUG
			"VK_LAYER_LUNARG_standard_validation",
#endif
			"", // std::array{} doesn't compile.
		};

		static const std::array extensions{
#ifndef NDEBUG
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_XCB_KHR
			VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
		};

		VkInstanceCreateInfo instance_info;
		instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_info.pNext = nullptr;
		instance_info.flags = 0;
		instance_info.pApplicationInfo = &application_info;
		instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size() - 1);
		instance_info.ppEnabledLayerNames = layers.data();
		instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instance_info.ppEnabledExtensionNames = extensions.data();

		VkInstance handle = VK_NULL_HANDLE;
		Y_VK_CHECK(vkCreateInstance(&instance_info, nullptr, &handle));
		return handle;
	}

#ifndef NDEBUG
	VKAPI_ATTR VkBool32 VKAPI_CALL print_vulkan_debug_report(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char* layer_prefix, const char* message, void*)
	{
		Yt::Logger::write(fmt::format("[{}] {}", layer_prefix, message));
		return VK_FALSE;
	}

	std::pair<VkDebugReportCallbackEXT, PFN_vkDestroyDebugReportCallbackEXT> create_vulkan_debug_report_callback(VkInstance instance)
	{
		const auto vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
		const auto vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
		if (!vkCreateDebugReportCallbackEXT || !vkDestroyDebugReportCallbackEXT)
			return { VK_NULL_HANDLE, nullptr };

		VkDebugReportCallbackCreateInfoEXT info;
		info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		info.pNext = nullptr;
		info.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
		info.pfnCallback = ::print_vulkan_debug_report;
		info.pUserData = nullptr;

		VkDebugReportCallbackEXT handle = VK_NULL_HANDLE;
		Y_VK_CHECK(vkCreateDebugReportCallbackEXT(instance, &info, nullptr, &handle));
		return { handle, vkDestroyDebugReportCallbackEXT };
	}
#endif

	VkSurfaceKHR create_vulkan_surface(VkInstance instance, const Yt::WindowID& window_id)
	{
#ifdef VK_USE_PLATFORM_WIN32_KHR
		VkWin32SurfaceCreateInfoKHR info;
		info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		info.hinstance = static_cast<HINSTANCE>(window_id._application);
		info.hwnd = reinterpret_cast<HWND>(window_id._window);

		VkSurfaceKHR handle = VK_NULL_HANDLE;
		Y_VK_CHECK(vkCreateWin32SurfaceKHR(instance, &info, nullptr, &handle));
		return handle;
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR info;
		info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		info.connection = static_cast<xcb_connection_t*>(window_id._application);
		info.window = static_cast<xcb_window_t>(window_id._window);

		VkSurfaceKHR handle = VK_NULL_HANDLE;
		Y_VK_CHECK(vkCreateXcbSurfaceKHR(instance, &info, nullptr, &handle));
		return handle;
#endif
	}

	std::pair<VkPhysicalDevice, uint32_t> select_vulkan_physical_device(VkInstance instance, VkSurfaceKHR surface)
	{
		uint32_t physical_device_count = 0;
		Y_VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr));

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		Y_VK_CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()));

		for (const auto physical_device : physical_devices)
		{
			uint32_t queue_family_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

			std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

			for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; ++queue_family_index)
			{
				if (!(queue_families[queue_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					continue;

				VkBool32 has_present_support = VK_FALSE;
				Y_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_index, surface, &has_present_support));
				if (has_present_support)
					return { physical_device, queue_family_index }; // TODO: Support separate queues for graphics and present.
			}
		}

		throw std::runtime_error{ "No suitable physical device found" };
	}

	void print_vulkan_texture_formats(VkPhysicalDevice device)
	{
		static const std::vector<std::pair<VkFormat, std::string_view>> formats{
			{ VK_FORMAT_R8_UNORM, "VK_FORMAT_R8_UNORM" },
			{ VK_FORMAT_R8G8_UNORM, "VK_FORMAT_R8G8_UNORM" },
			{ VK_FORMAT_R8G8B8_UNORM, "VK_FORMAT_R8G8B8_UNORM" },
			{ VK_FORMAT_B8G8R8_UNORM, "VK_FORMAT_B8G8R8_UNORM" },
			{ VK_FORMAT_R8G8B8A8_UNORM, "VK_FORMAT_R8G8B8A8_UNORM" },
			{ VK_FORMAT_B8G8R8A8_UNORM, "VK_FORMAT_B8G8R8A8_UNORM" },
			{ VK_FORMAT_R16_UNORM, "VK_FORMAT_R16_UNORM" },
			{ VK_FORMAT_R16G16_UNORM, "VK_FORMAT_R16G16_UNORM" },
			{ VK_FORMAT_R16G16B16_UNORM, "VK_FORMAT_R16G16B16_UNORM" },
			{ VK_FORMAT_R16G16B16A16_UNORM, "VK_FORMAT_R16G16B16A16_UNORM" },
		};

		Yt::Logger::write("Vulkan texture formats supported:");
		for (const auto& [format_id, format_name] : formats)
		{
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(device, format_id, &properties);
			Yt::Logger::write(fmt::format("  {} {} ({})", (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ? '+' : '-', format_name, format_id));
		}
		Yt::Logger::write("");
	}

	VkDevice create_vulkan_device(VkPhysicalDevice physical_device, uint32_t queue_family_index)
	{
		const float queue_prioritiy = 0.f;

		VkDeviceQueueCreateInfo queue_info;
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.pNext = nullptr;
		queue_info.flags = 0;
		queue_info.queueFamilyIndex = queue_family_index;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queue_prioritiy;

		static const std::initializer_list<const char*> extensions{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		VkDeviceCreateInfo device_info;
		device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_info.pNext = nullptr;
		device_info.flags = 0;
		device_info.queueCreateInfoCount = 1;
		device_info.pQueueCreateInfos = &queue_info;
		device_info.enabledLayerCount = 0;
		device_info.ppEnabledLayerNames = nullptr;
		device_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		device_info.ppEnabledExtensionNames = extensions.begin();
		device_info.pEnabledFeatures = nullptr;

		VkDevice handle = VK_NULL_HANDLE;
		Y_VK_CHECK(vkCreateDevice(physical_device, &device_info, nullptr, &handle));
		return handle;
	}

	VkCommandPool create_vulkan_command_pool(VkDevice device, uint32_t queue_family_index)
	{
		VkCommandPoolCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.queueFamilyIndex = queue_family_index;

		VkCommandPool handle = VK_NULL_HANDLE;
		Y_VK_CHECK(vkCreateCommandPool(device, &info, nullptr, &handle));
		return handle;
	}
}

namespace Yt
{
	VulkanContext::VulkanContext(const WindowID& window_id)
	{
		_data.create(window_id);
	}

	VK_CommandBuffer VulkanContext::allocate_command_buffer() const
	{
		return VK_CommandBuffer{ *this };
	}

	VK_HDeviceMemory VulkanContext::allocate_memory(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags flags) const
	{
		VkMemoryAllocateInfo info;
		info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.allocationSize = requirements.size;
		info.memoryTypeIndex = memory_type_index(requirements.memoryTypeBits, flags);

		VK_HDeviceMemory memory{ _data._device };
		memory.allocate(info);
		return memory;
	}

	VK_HImage VulkanContext::create_texture_2d_image(size_t width, size_t height, VkFormat format)
	{
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = format;
		info.extent.width = static_cast<uint32_t>(width);
		info.extent.height = static_cast<uint32_t>(height);
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VK_HImage image{ _data._device };
		image.create(info);
		return image;
	}

	VK_HSampler VulkanContext::create_texture_2d_sampler()
	{
		VkSamplerCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.magFilter = VK_FILTER_LINEAR;
		info.minFilter = VK_FILTER_LINEAR;
		info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		info.mipLodBias = 0.f;
		info.anisotropyEnable = _data._physical_device_features.samplerAnisotropy;
		info.maxAnisotropy = _data._physical_device_features.samplerAnisotropy ? 16.f : 1.f;
		info.compareEnable = VK_FALSE;
		info.compareOp = VK_COMPARE_OP_ALWAYS;
		info.minLod = 0.f;
		info.maxLod = 0.f;
		info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		info.unnormalizedCoordinates = VK_FALSE;

		VK_HSampler sampler{ _data._device };
		sampler.create(info);
		return sampler;
	}

	VK_HImageView VulkanContext::create_texture_2d_view(VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.image = image;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = format;
		info.components.r = VK_COMPONENT_SWIZZLE_R;
		info.components.g = VK_COMPONENT_SWIZZLE_G;
		info.components.b = VK_COMPONENT_SWIZZLE_B;
		info.components.a = VK_COMPONENT_SWIZZLE_A;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		VK_HImageView view{ _data._device };
		view.create(info);
		return view;
	}

	VkCompositeAlphaFlagBitsKHR VulkanContext::composite_alpha() const noexcept
	{
		static const VkCompositeAlphaFlagBitsKHR bits[]{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		const auto i = std::find_if(std::begin(bits), std::end(bits), [this](auto bit) {
			return _data._surface_capabilities.supportedCompositeAlpha & static_cast<VkFlags>(bit);
		});
		return i != std::end(bits) ? *i : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}

	uint32_t VulkanContext::memory_type_index(uint32_t type_bits, VkFlags flags) const
	{
		for (uint32_t i = 0; i < _data._physical_device_memory_properties.memoryTypeCount; ++i)
			if (type_bits & (1u << i))
				if ((_data._physical_device_memory_properties.memoryTypes[i].propertyFlags & flags) == flags)
					return i;
		throw std::runtime_error{ "No suitable memory type found" };
	}

	std::vector<VkSurfaceFormatKHR> VulkanContext::surface_formats() const
	{
		uint32_t count = 0;
		Y_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_data._physical_device, _data._surface, &count, nullptr));
		std::vector<VkSurfaceFormatKHR> formats(count);
		Y_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_data._physical_device, _data._surface, &count, formats.data()));
		if (formats.empty())
			throw std::runtime_error{ "No surface formats defined" };
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
			formats.clear();
		return formats;
	}

	VkSurfaceTransformFlagBitsKHR VulkanContext::surface_transform() const noexcept
	{
		return (_data._surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : _data._surface_capabilities.currentTransform;
	}

	VkImageTiling VulkanContext::tiling(VkFormat format, VkFlags flags) const
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(_data._physical_device, format, &properties);
		if ((properties.linearTilingFeatures & flags) == flags)
			return VK_IMAGE_TILING_LINEAR;
		if ((properties.optimalTilingFeatures & flags) == flags)
			return VK_IMAGE_TILING_OPTIMAL;
		throw std::runtime_error{ "Unsupported format" };
	}

	void VulkanContext::wait_idle() const
	{
		Y_VK_CHECK(vkDeviceWaitIdle(_data._device));
	}

	VulkanContext::Data::~Data() noexcept
	{
		if (_device != VK_NULL_HANDLE)
			vkDestroyDevice(_device, nullptr);
		if (_surface != VK_NULL_HANDLE)
			vkDestroySurfaceKHR(_instance, _surface, nullptr);
#ifndef NDEBUG
		if (_debug_report_callback != VK_NULL_HANDLE)
			_destroy_debug_report_callback(_instance, _debug_report_callback, nullptr);
#endif
		if (_instance != VK_NULL_HANDLE)
			vkDestroyInstance(_instance, nullptr);
	}

	void VulkanContext::Data::create(const WindowID& window_id)
	{
		::print_vulkan_layers_available();
		_instance = ::create_vulkan_instance();
#ifndef NDEBUG
		std::tie(_debug_report_callback, _destroy_debug_report_callback) = ::create_vulkan_debug_report_callback(_instance);
#endif
		_surface = ::create_vulkan_surface(_instance, window_id);
		std::tie(_physical_device, _queue_family_index) = ::select_vulkan_physical_device(_instance, _surface);
		::print_vulkan_texture_formats(_physical_device);
		vkGetPhysicalDeviceFeatures(_physical_device, &_physical_device_features);
		vkGetPhysicalDeviceMemoryProperties(_physical_device, &_physical_device_memory_properties);
		Y_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &_surface_capabilities));
		if (_surface_capabilities.currentExtent.width == 0xffffffff && _surface_capabilities.currentExtent.height == 0xffffffff)
			throw std::runtime_error{ "Bad surface size" };
		_device = ::create_vulkan_device(_physical_device, _queue_family_index);
		vkGetDeviceQueue(_device, _queue_family_index, 0, &_queue);
		_command_pool = ::create_vulkan_command_pool(_device, _queue_family_index);
	}
}
