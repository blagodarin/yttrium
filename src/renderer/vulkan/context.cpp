#include "context.h"

#include "../../system/window.h"

#include <stdexcept>

namespace
{
	const auto vulkan_instance_extensions =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_XCB_KHR
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	};

	const auto vulkan_device_extensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	void vulkan_throw(VkResult result, const std::string& function)
	{
		const auto result_to_string = [result]() -> std::string
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
			case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHX: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHX";
			default: return std::to_string(result);
			}
		};
		throw std::runtime_error{function + " = " + result_to_string()};
	}
}

namespace Yttrium
{
	void VulkanContext::initialize(const WindowBackend& window)
	{
		reset();

		VkApplicationInfo ai = {};
		ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		ai.pNext = nullptr;
		ai.pApplicationName = nullptr;
		ai.applicationVersion = 0;
		ai.pEngineName = nullptr;
		ai.engineVersion = 0;
		ai.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instance_ci = {};
		instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_ci.pNext = nullptr;
		instance_ci.flags = 0;
		instance_ci.pApplicationInfo = &ai;
		instance_ci.enabledLayerCount = 0;
		instance_ci.ppEnabledLayerNames = nullptr;
		instance_ci.enabledExtensionCount = vulkan_instance_extensions.size();
		instance_ci.ppEnabledExtensionNames = vulkan_instance_extensions.begin();

		if (const auto result = vkCreateInstance(&instance_ci, nullptr, &_instance))
			vulkan_throw(result, "vkCreateInstance");

#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR surface_ci = {};
		surface_ci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		surface_ci.connection = window.xcb_connection();
		surface_ci.window = window.xcb_window();
		if (const auto result = vkCreateXcbSurfaceKHR(_instance, &surface_ci, nullptr, &_surface))
			vulkan_throw(result, "vkCreateXcbSurfaceKHR");
#endif

		uint32_t physical_device_count = 0;
		if (const auto result = vkEnumeratePhysicalDevices(_instance, &physical_device_count, nullptr))
			vulkan_throw(result, "vkEnumeratePhysicalDevices");

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		if (const auto result = vkEnumeratePhysicalDevices(_instance, &physical_device_count, physical_devices.data()))
			vulkan_throw(result, "vkEnumeratePhysicalDevices");

		const float queue_prioritiy = 0.f;

		VkDeviceQueueCreateInfo device_queue_ci = {};
		device_queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		device_queue_ci.pNext = nullptr;
		device_queue_ci.flags = 0;
		device_queue_ci.queueFamilyIndex = 0;
		device_queue_ci.queueCount = 1;
		device_queue_ci.pQueuePriorities = &queue_prioritiy;

		for (const auto physical_device : physical_devices)
		{
			uint32_t queue_family_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

			std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

			for (uint32_t i = 0; i < queue_family_count; ++i)
			{
				if (!(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					continue;

				VkBool32 has_present_support = VK_FALSE;
				if (const auto result = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, _surface, &has_present_support))
					vulkan_throw(result, "vkGetPhysicalDeviceSurfaceSupportKHR");

				if (!has_present_support)
					continue;

				_physical_device = physical_device;
				device_queue_ci.queueFamilyIndex = i; // TODO: Support separate queues for graphics and present.
				break;
			}
		}

		if (_physical_device == VK_NULL_HANDLE)
			throw std::runtime_error{"No suitable physical device found"};

		VkSurfaceCapabilitiesKHR surface_caps = {};
		if (const auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &surface_caps))
			vulkan_throw(result, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

		if (surface_caps.currentExtent.width == 0xffffffff && surface_caps.currentExtent.height == 0xffffffff)
			throw std::runtime_error{"Bad surface size"};

		uint32_t surface_format_count = 0;
		if (const auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface, &surface_format_count, nullptr))
			vulkan_throw(result, "vkGetPhysicalDeviceSurfaceFormatsKHR");

		std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
		if (const auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface, &surface_format_count, surface_formats.data()))
			vulkan_throw(result, "vkGetPhysicalDeviceSurfaceFormatsKHR");

		if (surface_formats.empty())
			throw std::runtime_error{"No surface formats defined"};
		else if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
			surface_formats.clear();

		VkDeviceCreateInfo device_ci = {};
		device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_ci.pNext = nullptr;
		device_ci.flags = 0;
		device_ci.queueCreateInfoCount = 1;
		device_ci.pQueueCreateInfos = &device_queue_ci;
		device_ci.enabledLayerCount = 0;
		device_ci.ppEnabledLayerNames = nullptr;
		device_ci.enabledExtensionCount = vulkan_device_extensions.size();
		device_ci.ppEnabledExtensionNames = vulkan_device_extensions.begin();
		device_ci.pEnabledFeatures = nullptr;
		if (const auto result = vkCreateDevice(_physical_device, &device_ci, nullptr, &_device))
			vulkan_throw(result, "vkCreateDevice");

		VkSwapchainCreateInfoKHR swapchain_ci = {};
		swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_ci.pNext = nullptr;
		swapchain_ci.flags = 0;
		swapchain_ci.surface = _surface;
		swapchain_ci.minImageCount = surface_caps.minImageCount;
		swapchain_ci.imageFormat = surface_formats.empty() ? VK_FORMAT_B8G8R8A8_UNORM : surface_formats[0].format;
		swapchain_ci.imageColorSpace = surface_formats.empty() ? VK_COLOR_SPACE_SRGB_NONLINEAR_KHR : surface_formats[0].colorSpace;
		swapchain_ci.imageExtent = surface_caps.currentExtent;
		swapchain_ci.imageArrayLayers = 1;
		swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_ci.queueFamilyIndexCount = 0;
		swapchain_ci.pQueueFamilyIndices = nullptr;
		swapchain_ci.preTransform = surface_caps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surface_caps.currentTransform;
		swapchain_ci.compositeAlpha = [&surface_caps]
		{
			for (const auto bit : {VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR})
				if (surface_caps.supportedCompositeAlpha & static_cast<VkFlags>(bit))
					return bit;
			return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		}();
		swapchain_ci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchain_ci.clipped = VK_TRUE;
		swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
		if (const auto result = vkCreateSwapchainKHR(_device, &swapchain_ci, nullptr, &_swapchain))
			vulkan_throw(result, "vkCreateSwapchainKHR");

		uint32_t swapchain_image_count = 0;
		if (const auto result = vkGetSwapchainImagesKHR(_device, _swapchain, &swapchain_image_count, nullptr))
			vulkan_throw(result, "vkGetSwapchainImagesKHR");

		std::vector<VkImage> swapchain_images(swapchain_image_count);
		if (const auto result = vkGetSwapchainImagesKHR(_device, _swapchain, &swapchain_image_count, swapchain_images.data()))
			vulkan_throw(result, "vkGetSwapchainImagesKHR");

		_image_views.resize(swapchain_image_count, VK_NULL_HANDLE);
		for (uint32_t i = 0; i < swapchain_image_count; ++i)
		{
			VkImageViewCreateInfo image_view_ci = {};
			image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			image_view_ci.pNext = nullptr;
			image_view_ci.flags = 0;
			image_view_ci.image = swapchain_images[i];
			image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
			image_view_ci.format = swapchain_ci.imageFormat;
			image_view_ci.components.r = VK_COMPONENT_SWIZZLE_R;
			image_view_ci.components.g = VK_COMPONENT_SWIZZLE_G;
			image_view_ci.components.b = VK_COMPONENT_SWIZZLE_B;
			image_view_ci.components.a = VK_COMPONENT_SWIZZLE_A;
			image_view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_view_ci.subresourceRange.baseMipLevel = 0;
			image_view_ci.subresourceRange.levelCount = 1;
			image_view_ci.subresourceRange.baseArrayLayer = 0;
			image_view_ci.subresourceRange.layerCount = 1;
			if (const auto result = vkCreateImageView(_device, &image_view_ci, nullptr, &_image_views[i]))
				vulkan_throw(result, "vkCreateImageView");
		}

		VkImageCreateInfo depth_image_ci = {};
		depth_image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depth_image_ci.pNext = nullptr;
		depth_image_ci.flags = 0;
		depth_image_ci.imageType = VK_IMAGE_TYPE_2D;
		depth_image_ci.format = VK_FORMAT_D16_UNORM;
		depth_image_ci.extent.width = surface_caps.currentExtent.width;
		depth_image_ci.extent.height = surface_caps.currentExtent.height;
		depth_image_ci.extent.depth = 1;
		depth_image_ci.mipLevels = 1;
		depth_image_ci.arrayLayers = 1;
		depth_image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_image_ci.tiling = [this, &depth_image_ci]
		{
			VkFormatProperties depth_format_props = {};
			vkGetPhysicalDeviceFormatProperties(_physical_device, depth_image_ci.format, &depth_format_props);
			if (depth_format_props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return VK_IMAGE_TILING_LINEAR;
			else if (depth_format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return VK_IMAGE_TILING_OPTIMAL;
			else
				throw std::runtime_error{"VK_FORMAT_D16_UNORM is not supported"};
		}();
		depth_image_ci.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depth_image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		depth_image_ci.queueFamilyIndexCount = 0;
		depth_image_ci.pQueueFamilyIndices = nullptr;
		depth_image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (const auto result = vkCreateImage(_device, &depth_image_ci, nullptr, &_depth_image))
			vulkan_throw(result, "vkCreateImage");

		VkMemoryRequirements depth_memory_reqs = {};
		vkGetImageMemoryRequirements(_device, _depth_image, &depth_memory_reqs);

		VkMemoryAllocateInfo depth_memory_ai = {};
		depth_memory_ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		depth_memory_ai.pNext = nullptr;
		depth_memory_ai.allocationSize = depth_memory_reqs.size;
		depth_memory_ai.memoryTypeIndex = [this, &depth_memory_reqs]
		{
			VkPhysicalDeviceMemoryProperties props;
			vkGetPhysicalDeviceMemoryProperties(_physical_device, &props);
			for (uint32_t i = 0; i < props.memoryTypeCount; ++i)
				if (depth_memory_reqs.memoryTypeBits & (1u << i))
					if (props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
						return i;
			throw std::runtime_error{"VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT memory not found"};
		}();
		if (const auto result = vkAllocateMemory(_device, &depth_memory_ai, nullptr, &_depth_memory))
			vulkan_throw(result, "vkAllocateMemory");

		if (const auto result = vkBindImageMemory(_device, _depth_image, _depth_memory, 0))
			vulkan_throw(result, "vkBindImageMemory");

		VkImageViewCreateInfo depth_image_view_ci = {};
		depth_image_view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depth_image_view_ci.pNext = nullptr;
		depth_image_view_ci.flags = 0;
		depth_image_view_ci.image = _depth_image;
		depth_image_view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depth_image_view_ci.format = depth_image_ci.format;
		depth_image_view_ci.components.r = VK_COMPONENT_SWIZZLE_R;
		depth_image_view_ci.components.g = VK_COMPONENT_SWIZZLE_G;
		depth_image_view_ci.components.b = VK_COMPONENT_SWIZZLE_B;
		depth_image_view_ci.components.a = VK_COMPONENT_SWIZZLE_A;
		depth_image_view_ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		depth_image_view_ci.subresourceRange.baseMipLevel = 0;
		depth_image_view_ci.subresourceRange.levelCount = 1;
		depth_image_view_ci.subresourceRange.baseArrayLayer = 0;
		depth_image_view_ci.subresourceRange.layerCount = 1;
		if (const auto result = vkCreateImageView(_device, &depth_image_view_ci, nullptr, &_depth_image_view))
			vulkan_throw(result, "vkCreateImageView");

		VkCommandPoolCreateInfo command_pool_ci = {};
		command_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_ci.pNext = nullptr;
		command_pool_ci.flags = 0;
		command_pool_ci.queueFamilyIndex = device_queue_ci.queueFamilyIndex;
		if (const auto result = vkCreateCommandPool(_device, &command_pool_ci, nullptr, &_command_pool))
			vulkan_throw(result, "vkCreateCommandPool");

		VkCommandBufferAllocateInfo command_buffer_ai = {};
		command_buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_ai.pNext = nullptr;
		command_buffer_ai.commandPool = _command_pool;
		command_buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_ai.commandBufferCount = 1;
		if (const auto result = vkAllocateCommandBuffers(_device, &command_buffer_ai, &_command_buffer))
			vulkan_throw(result, "vkAllocateCommandBuffers");
	}

	void VulkanContext::reset() noexcept
	{
		if (_command_buffer != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(_device, _command_pool, 1, &_command_buffer);
			_command_buffer = VK_NULL_HANDLE;
		}
		if (_command_pool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(_device, _command_pool, nullptr);
			_command_pool = VK_NULL_HANDLE;
		}
		if (_depth_image_view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(_device, _depth_image_view, nullptr);
			_depth_image_view = VK_NULL_HANDLE;
		}
		if (_depth_memory != VK_NULL_HANDLE)
		{
			vkFreeMemory(_device, _depth_memory, nullptr);
			_depth_memory = VK_NULL_HANDLE;
		}
		if (_depth_image != VK_NULL_HANDLE)
		{
			vkDestroyImage(_device, _depth_image, nullptr);
			_depth_image = VK_NULL_HANDLE;
		}
		for (auto i = _image_views.rbegin(); i != _image_views.rend(); ++i)
			if (*i != VK_NULL_HANDLE)
				vkDestroyImageView(_device, *i, nullptr);
		_image_views.clear();
		if (_swapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(_device, _swapchain, nullptr);
			_swapchain = VK_NULL_HANDLE;
		}
		if (_device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(_device, nullptr);
			_device = VK_NULL_HANDLE;
		}
		if (_surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(_instance, _surface, nullptr);
			_surface = VK_NULL_HANDLE;
		}
		if (_instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(_instance, nullptr);
			_instance = VK_NULL_HANDLE;
		}
	}
}
