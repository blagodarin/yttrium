#include "wrappers.h"

#include "../../system/window.h"

#include <stdexcept>

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

		switch (vkCreateInstance(&ici, nullptr, &_instance))
		{
		case VK_SUCCESS:
			return;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			throw std::runtime_error{"Vulkan ICD not found"};
		default:
			throw std::runtime_error{"vkCreateInstance error"};
		}
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
		if (VK_SUCCESS != vkCreateXcbSurfaceKHR(_instance, &sci, nullptr, &_surface))
			throw std::runtime_error{"vkCreateXcbSurfaceKHR"};
#endif
	}

	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
	}
}
