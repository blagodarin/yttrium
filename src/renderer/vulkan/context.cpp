#include "context.h"

#include "../../system/window.h"

#include <stdexcept>
#include <vector>

namespace
{
	const std::vector<const char*> vulkan_instance_extensions
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_XCB_KHR
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	};

	const std::vector<const char*> vulkan_device_extensions
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

		VkInstanceCreateInfo ici = {};
		ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		ici.pNext = nullptr;
		ici.flags = 0;
		ici.pApplicationInfo = &ai;
		ici.enabledLayerCount = 0;
		ici.ppEnabledLayerNames = nullptr;
		ici.enabledExtensionCount = vulkan_instance_extensions.size();
		ici.ppEnabledExtensionNames = vulkan_instance_extensions.data();

		if (const auto result = vkCreateInstance(&ici, nullptr, &_instance))
			vulkan_throw(result, "vkCreateInstance");

#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR sci = {};
		sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		sci.connection = window.xcb_connection();
		sci.window = window.xcb_window();
		if (const auto result = vkCreateXcbSurfaceKHR(_instance, &sci, nullptr, &_surface))
			vulkan_throw(result, "vkCreateXcbSurfaceKHR");
#endif

		uint32_t physical_device_count = 0;
		if (const auto result = vkEnumeratePhysicalDevices(_instance, &physical_device_count, nullptr))
			vulkan_throw(result, "vkEnumeratePhysicalDevices");
		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		if (const auto result = vkEnumeratePhysicalDevices(_instance, &physical_device_count, physical_devices.data()))
			vulkan_throw(result, "vkEnumeratePhysicalDevices");

		const float queue_prioritiy = 0.f;

		VkDeviceQueueCreateInfo dqci = {};
		dqci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		dqci.pNext = nullptr;
		dqci.flags = 0;
		dqci.queueFamilyIndex = 0;
		dqci.queueCount = 1;
		dqci.pQueuePriorities = &queue_prioritiy;

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
				dqci.queueFamilyIndex = i;
				break;
			}
		}

		if (_physical_device == VK_NULL_HANDLE)
			throw std::runtime_error{"No suitable physical device found"};

		VkDeviceCreateInfo dci = {};
		dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		dci.pNext = nullptr;
		dci.flags = 0;
		dci.queueCreateInfoCount = 1;
		dci.pQueueCreateInfos = &dqci;
		dci.enabledLayerCount = 0;
		dci.ppEnabledLayerNames = nullptr;
		dci.enabledExtensionCount = vulkan_device_extensions.size();
		dci.ppEnabledExtensionNames = vulkan_device_extensions.data();
		dci.pEnabledFeatures = nullptr;
		if (const auto result = vkCreateDevice(_physical_device, &dci, nullptr, &_device))
			vulkan_throw(result, "vkCreateDevice");

		VkCommandPoolCreateInfo cpci = {};
		cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cpci.pNext = nullptr;
		cpci.flags = 0;
		cpci.queueFamilyIndex = dqci.queueFamilyIndex;
		if (const auto result = vkCreateCommandPool(_device, &cpci, nullptr, &_command_pool))
			vulkan_throw(result, "vkCreateCommandPool");

		VkCommandBufferAllocateInfo cbai = {};
		cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbai.pNext = nullptr;
		cbai.commandPool = _command_pool;
		cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cbai.commandBufferCount = 1;
		if (const auto result = vkAllocateCommandBuffers(_device, &cbai, &_command_buffer))
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
