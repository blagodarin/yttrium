#include "wrappers.h"

#include "../../system/window.h"

#include <stdexcept>

namespace
{
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
	VulkanInstance::VulkanInstance()
	{
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
		ici.enabledExtensionCount = 0;
		ici.ppEnabledExtensionNames = nullptr;
		ici.enabledLayerCount = 0;
		ici.ppEnabledLayerNames = nullptr;

		if (const auto result = vkCreateInstance(&ici, nullptr, &_instance))
			vulkan_throw(result, "vkCreateInstance");
	}

	VulkanInstance::~VulkanInstance()
	{
		vkDestroyInstance(_instance, nullptr);
	}

	VulkanSurface::VulkanSurface(VkInstance instance, const WindowBackend& backend)
		: _instance{instance}
	{
#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR sci = {};
		sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		sci.connection = backend.xcb_connection();
		sci.window = backend.xcb_window();
		if (const auto result = vkCreateXcbSurfaceKHR(_instance, &sci, nullptr, &_surface))
			vulkan_throw(result, "vkCreateXcbSurfaceKHR");
#endif
	}

	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	}
}
