#include "context.h"

#include <yttrium/math/matrix.h>
#include "../../system/window.h"
#include "glsl.h"

#include <cassert>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace
{
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

	void vulkan_throw(VkResult result, const std::string& function)
	{
		throw std::runtime_error{function.substr(0, function.find('(')) + " = " + ::vulkan_result_to_string(result)};
	}

#define CHECK(call) if (const auto result = (call)) vulkan_throw(result, #call);

	VkShaderModule create_glsl_shader(VkDevice device, VkShaderStageFlagBits type, const char* source)
	{
		const auto spirv = Yttrium::glsl_to_spirv(source, type);

		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.codeSize = spirv.size() * sizeof(uint32_t);
		create_info.pCode = spirv.data();

		VkShaderModule shader = VK_NULL_HANDLE;
		CHECK(vkCreateShaderModule(device, &create_info, nullptr, &shader));
		return shader;
	}
}

namespace Yttrium
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Instance::VK_Instance()
	{
		VkApplicationInfo application_info = {};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		application_info.pApplicationName = nullptr;
		application_info.applicationVersion = 0;
		application_info.pEngineName = nullptr;
		application_info.engineVersion = 0;
		application_info.apiVersion = VK_API_VERSION_1_0;

		static const auto extensions =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_XCB_KHR
			VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
		};

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.pApplicationInfo = &application_info;
		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = nullptr;
		create_info.enabledExtensionCount = extensions.size();
		create_info.ppEnabledExtensionNames = extensions.begin();

		CHECK(vkCreateInstance(&create_info, nullptr, &_handle));
	}

	VK_Instance::~VK_Instance() noexcept
	{
		vkDestroyInstance(_handle, nullptr);
	}

	std::vector<VkPhysicalDevice> VK_Instance::physical_device_handles() const
	{
		uint32_t count = 0;
		CHECK(vkEnumeratePhysicalDevices(_handle, &count, nullptr));
		std::vector<VkPhysicalDevice> handles(count);
		CHECK(vkEnumeratePhysicalDevices(_handle, &count, handles.data()));
		return handles;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Surface::VK_Surface(const VK_Instance& instance, const WindowBackend& window)
		: _instance{instance}
	{
#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		create_info.connection = window.xcb_connection();
		create_info.window = window.xcb_window();

		CHECK(vkCreateXcbSurfaceKHR(_instance._handle, &create_info, nullptr, &_handle));
#endif
	}

	VK_Surface::~VK_Surface() noexcept
	{
		vkDestroySurfaceKHR(_instance._handle, _handle, nullptr);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_PhysicalDevice::VK_PhysicalDevice(const VK_Surface& surface)
		: _surface{surface}
	{
		for (const auto handle : _surface._instance.physical_device_handles())
		{
			uint32_t queue_family_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(handle, &queue_family_count, nullptr);

			std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
			vkGetPhysicalDeviceQueueFamilyProperties(handle, &queue_family_count, queue_families.data());

			for (uint32_t i = 0; i < queue_family_count; ++i)
			{
				if (!(queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					continue;

				VkBool32 has_present_support = VK_FALSE;
				CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(handle, i, _surface._handle, &has_present_support));
				if (has_present_support)
				{
					_handle = handle;
					_queue_family_index = i; // TODO: Support separate queues for graphics and present.
					break;
				}
			}

			if (_handle != VK_NULL_HANDLE)
				break;
		}

		if (_handle == VK_NULL_HANDLE)
			throw std::runtime_error{"No suitable physical device found"};

		CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_handle, _surface._handle, &_surface_capabilities));
		if (_surface_capabilities.currentExtent.width == 0xffffffff && _surface_capabilities.currentExtent.height == 0xffffffff)
			throw std::runtime_error{"Bad surface size"};

		vkGetPhysicalDeviceMemoryProperties(_handle, &_memory_properties);
	}

	VkCompositeAlphaFlagBitsKHR VK_PhysicalDevice::composite_alpha() const noexcept
	{
		for (const auto bit : {VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR})
			if (_surface_capabilities.supportedCompositeAlpha & static_cast<VkFlags>(bit))
				return bit;
		return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	}

	uint32_t VK_PhysicalDevice::memory_type_index(uint32_t type_bits, VkFlags flags) const
	{
		for (uint32_t i = 0; i < _memory_properties.memoryTypeCount; ++i)
			if (type_bits & (1u << i))
				if ((_memory_properties.memoryTypes[i].propertyFlags & flags) == flags)
					return i;
		throw std::runtime_error{"No suitable memory type found"};
	}

	std::vector<VkSurfaceFormatKHR> VK_PhysicalDevice::surface_formats() const
	{
		uint32_t count = 0;
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_handle, _surface._handle, &count, nullptr));
		std::vector<VkSurfaceFormatKHR> formats(count);
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_handle, _surface._handle, &count, formats.data()));
		if (formats.empty())
			throw std::runtime_error{"No surface formats defined"};
		else if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
			formats.clear();
		return formats;
	}

	VkSurfaceTransformFlagBitsKHR VK_PhysicalDevice::surface_transform() const noexcept
	{
		return (_surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : _surface_capabilities.currentTransform;
	}

	VkImageTiling VK_PhysicalDevice::tiling(VkFormat format, VkFlags flags) const
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(_handle, format, &properties);
		if ((properties.linearTilingFeatures & flags) == flags)
			return VK_IMAGE_TILING_LINEAR;
		else if ((properties.optimalTilingFeatures & flags) == flags)
			return VK_IMAGE_TILING_OPTIMAL;
		else
			throw std::runtime_error{"Unsupported format"};
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Device::VK_Device(const VK_PhysicalDevice& physical_device)
		: _physical_device{physical_device}
	{
		const float queue_prioritiy = 0.f;

		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.pNext = nullptr;
		queue_create_info.flags = 0;
		queue_create_info.queueFamilyIndex = _physical_device._queue_family_index;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_prioritiy;

		static const auto extensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.queueCreateInfoCount = 1;
		create_info.pQueueCreateInfos = &queue_create_info;
		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = nullptr;
		create_info.enabledExtensionCount = extensions.size();
		create_info.ppEnabledExtensionNames = extensions.begin();
		create_info.pEnabledFeatures = nullptr;

		CHECK(vkCreateDevice(_physical_device._handle, &create_info, nullptr, &_handle));

		vkGetDeviceQueue(_handle, _physical_device._queue_family_index, 0, &_graphics_queue);
		_present_queue = _graphics_queue;
	}

	VK_Device::~VK_Device() noexcept
	{
		vkDestroyDevice(_handle, nullptr);
	}

	VkDeviceMemory VK_Device::allocate_memory(const VkMemoryRequirements& requirements, VkFlags flags) const
	{
		VkMemoryAllocateInfo mai = {};
		mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mai.pNext = nullptr;
		mai.allocationSize = requirements.size;
		mai.memoryTypeIndex = _physical_device.memory_type_index(requirements.memoryTypeBits, flags);
		VkDeviceMemory handle = VK_NULL_HANDLE;
		CHECK(vkAllocateMemory(_handle, &mai, nullptr, &handle));
		return handle;
	}

	void VK_Device::wait_idle()
	{
		CHECK(vkDeviceWaitIdle(_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Swapchain::~VK_Swapchain() noexcept
	{
		for (const auto view : _views)
			vkDestroyImageView(_device._handle, view, nullptr);
		if (_handle != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(_device._handle, _handle, nullptr);
	}

	void VK_Swapchain::create()
	{
		assert(_handle == VK_NULL_HANDLE);

		const auto surface_formats = _device._physical_device.surface_formats();
		_format = surface_formats.empty() ? VK_FORMAT_B8G8R8A8_UNORM : surface_formats[0].format;

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.surface = _device._physical_device._surface._handle;
		create_info.minImageCount = _device._physical_device._surface_capabilities.minImageCount;
		create_info.imageFormat = _format;
		create_info.imageColorSpace = surface_formats.empty() ? VK_COLOR_SPACE_SRGB_NONLINEAR_KHR : surface_formats[0].colorSpace;
		create_info.imageExtent = _device._physical_device._surface_capabilities.currentExtent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
		create_info.preTransform = _device._physical_device.surface_transform();
		create_info.compositeAlpha = _device._physical_device.composite_alpha();
		create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		CHECK(vkCreateSwapchainKHR(_device._handle, &create_info, nullptr, &_handle));
	}

	void VK_Swapchain::create_views()
	{
		assert(_handle != VK_NULL_HANDLE && _views.empty());

		uint32_t image_count = 0;
		CHECK(vkGetSwapchainImagesKHR(_device._handle, _handle, &image_count, nullptr));

		std::vector<VkImage> images(image_count);
		CHECK(vkGetSwapchainImagesKHR(_device._handle, _handle, &image_count, images.data()));

		_views.reserve(image_count);
		for (const auto image : images)
		{
			VkImageViewCreateInfo create_info;
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.pNext = nullptr;
			create_info.flags = 0;
			create_info.image = image;
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = _format;
			create_info.components.r = VK_COMPONENT_SWIZZLE_R;
			create_info.components.g = VK_COMPONENT_SWIZZLE_G;
			create_info.components.b = VK_COMPONENT_SWIZZLE_B;
			create_info.components.a = VK_COMPONENT_SWIZZLE_A;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			VkImageView view = VK_NULL_HANDLE;
			CHECK(vkCreateImageView(_device._handle, &create_info, nullptr, &view));
			_views.emplace_back(view);
		}
	}

	VkAttachmentDescription VK_Swapchain::attachment_description() const noexcept
	{
		VkAttachmentDescription description;
		description.flags = 0;
		description.format = _format;
		description.samples = VK_SAMPLE_COUNT_1_BIT;
		description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return description;
	}

	uint32_t VK_Swapchain::acquire_next_image(VkSemaphore semaphore) const
	{
		uint32_t index = 0;
		switch (const auto result = vkAcquireNextImageKHR(_device._handle, _handle, std::numeric_limits<uint64_t>::max(), semaphore, VK_NULL_HANDLE, &index))
		{
		case VK_SUCCESS: return index;
		case VK_ERROR_OUT_OF_DATE_KHR: throw OutOfDate{};
		default: throw std::runtime_error{"vkAcquireNextImageKHR = " + ::vulkan_result_to_string(result)};
		}
	}

	void VK_Swapchain::present(uint32_t framebuffer_index, VkSemaphore semaphore) const
	{
		VkPresentInfoKHR present_info;
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.pNext = nullptr;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &semaphore;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &_handle;
		present_info.pImageIndices = &framebuffer_index;
		present_info.pResults = nullptr;

		switch (const auto result = vkQueuePresentKHR(_device._present_queue, &present_info))
		{
		case VK_SUCCESS: return;
		case VK_ERROR_OUT_OF_DATE_KHR: throw OutOfDate{};
		default: throw std::runtime_error{"vkQueuePresentKHR = " + ::vulkan_result_to_string(result)};
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DepthBuffer::~VK_DepthBuffer() noexcept
	{
		if (_view != VK_NULL_HANDLE)
			vkDestroyImageView(_device._handle, _view, nullptr);
		if (_image != VK_NULL_HANDLE)
			vkDestroyImage(_device._handle, _image, nullptr);
		if (_memory != VK_NULL_HANDLE)
			vkFreeMemory(_device._handle, _memory, nullptr);
	}

	void VK_DepthBuffer::create_image(VkFormat format)
	{
		assert(_image == VK_NULL_HANDLE);

		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.format = format;
		create_info.extent.width = _device._physical_device._surface_capabilities.currentExtent.width;
		create_info.extent.height = _device._physical_device._surface_capabilities.currentExtent.height;
		create_info.extent.depth = 1;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		create_info.tiling = _device._physical_device.tiling(format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
		create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		CHECK(vkCreateImage(_device._handle, &create_info, nullptr, &_image));

		_format = format;
	}

	void VK_DepthBuffer::allocate_memory(VkFlags flags)
	{
		assert(_image != VK_NULL_HANDLE && _memory == VK_NULL_HANDLE);

		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements(_device._handle, _image, &memory_requirements);
		_memory = _device.allocate_memory(memory_requirements, flags);
		CHECK(vkBindImageMemory(_device._handle, _image, _memory, 0));
	}

	void VK_DepthBuffer::create_view()
	{
		assert(_image != VK_NULL_HANDLE && _view == VK_NULL_HANDLE);

		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.image = _image;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = _format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_R;
		create_info.components.g = VK_COMPONENT_SWIZZLE_G;
		create_info.components.b = VK_COMPONENT_SWIZZLE_B;
		create_info.components.a = VK_COMPONENT_SWIZZLE_A;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		CHECK(vkCreateImageView(_device._handle, &create_info, nullptr, &_view));
	}

	VkAttachmentDescription VK_DepthBuffer::attachment_description() const noexcept
	{
		VkAttachmentDescription description;
		description.flags = 0;
		description.format = _format;
		description.samples = VK_SAMPLE_COUNT_1_BIT;
		description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		return description;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_RenderPass::~VK_RenderPass() noexcept
	{
		if (_handle != VK_NULL_HANDLE)
			vkDestroyRenderPass(_device._handle, _handle, nullptr);
	}

	void VK_RenderPass::create(const VK_Swapchain& swapchain, const VK_DepthBuffer& depth_buffer)
	{
		assert(&_device == &swapchain._device && &_device == &depth_buffer._device);
		assert(_handle == VK_NULL_HANDLE);

		const std::array<VkAttachmentDescription, 2> attachment_descriptions{swapchain.attachment_description(), depth_buffer.attachment_description()};

		VkAttachmentReference color_reference = {};
		color_reference.attachment = 0;
		color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_reference = {};
		depth_reference.attachment = 1;
		depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.flags = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_reference;
		subpass.pResolveAttachments = nullptr;
		subpass.pDepthStencilAttachment = &depth_reference;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;

		VkRenderPassCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.attachmentCount = attachment_descriptions.size();
		create_info.pAttachments = attachment_descriptions.data();
		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass;
		create_info.dependencyCount = 0;
		create_info.pDependencies = nullptr;

		CHECK(vkCreateRenderPass(_device._handle, &create_info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Framebuffers::~VK_Framebuffers() noexcept
	{
		for (const auto handle : _handles)
			vkDestroyFramebuffer(_device._handle, handle, nullptr);
	}

	void VK_Framebuffers::create(const VK_RenderPass& render_pass, const VK_Swapchain& swapchain, const VK_DepthBuffer& depth_buffer)
	{
		std::array<VkImageView, 2> attachments{VK_NULL_HANDLE, depth_buffer._view};

		VkFramebufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.renderPass = render_pass._handle;
		create_info.attachmentCount = attachments.size();
		create_info.pAttachments = attachments.data();
		create_info.width = _device._physical_device._surface_capabilities.currentExtent.width;
		create_info.height = _device._physical_device._surface_capabilities.currentExtent.height;
		create_info.layers = 1;

		_handles.reserve(swapchain._views.size());
		for (const auto swapchain_view : swapchain._views)
		{
			attachments[0] = swapchain_view;

			VkFramebuffer framebuffer = VK_NULL_HANDLE;
			CHECK(vkCreateFramebuffer(_device._handle, &create_info, nullptr, &framebuffer));
			_handles.emplace_back(framebuffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Buffer::~VK_Buffer() noexcept
	{
		if (_handle != VK_NULL_HANDLE)
			vkDestroyBuffer(_device._handle, _handle, nullptr);
		if (_memory != VK_NULL_HANDLE)
			vkFreeMemory(_device._handle, _memory, nullptr);
	}

	void VK_Buffer::create(VkBufferUsageFlags usage)
	{
		assert(_handle == VK_NULL_HANDLE);

		VkBufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.size = _size;
		create_info.usage = usage;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;

		CHECK(vkCreateBuffer(_device._handle, &create_info, nullptr, &_handle));
	}

	void VK_Buffer::allocate_memory(VkFlags flags)
	{
		assert(_handle != VK_NULL_HANDLE && _memory == VK_NULL_HANDLE);

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(_device._handle, _handle, &memory_requirements);
		_memory = _device.allocate_memory(memory_requirements, flags);
		CHECK(vkBindBufferMemory(_device._handle, _handle, _memory, 0));
	}

	VkDescriptorBufferInfo VK_Buffer::descriptor_buffer_info() const noexcept
	{
		VkDescriptorBufferInfo result;
		result.buffer = _handle;
		result.offset = 0;
		result.range = _size;
		return result;
	}

	void VK_Buffer::write(const void* data, size_t size)
	{
		assert(_memory != VK_NULL_HANDLE);

		void* mapped_memory = nullptr;
		CHECK(vkMapMemory(_device._handle, _memory, 0, size, 0, &mapped_memory));
		std::memcpy(mapped_memory, data, size);
		vkUnmapMemory(_device._handle, _memory);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Semaphore::VK_Semaphore(const VK_Device& device)
		: _device{device}
	{
		VkSemaphoreCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		CHECK(vkCreateSemaphore(_device._handle, &create_info, nullptr, &_handle));
	}

	VK_Semaphore::~VK_Semaphore() noexcept
	{
		vkDestroySemaphore(_device._handle, _handle, nullptr);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DescriptorSetLayout::VK_DescriptorSetLayout(const VK_Device& device, std::vector<Binding>&& bindings)
		: _device{device}
	{
		for (auto& binding : bindings)
			binding.binding = &binding - bindings.data();

		VkDescriptorSetLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.bindingCount = bindings.size();
		create_info.pBindings = bindings.data();
		CHECK(vkCreateDescriptorSetLayout(_device._handle, &create_info, nullptr, &_handle));
	}

	VK_DescriptorSetLayout::~VK_DescriptorSetLayout() noexcept
	{
		vkDestroyDescriptorSetLayout(_device._handle, _handle, nullptr);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_PipelineLayout::VK_PipelineLayout(const VK_Device& device, std::initializer_list<VkDescriptorSetLayout> set_layouts)
		: _device{device}
	{
		VkPipelineLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.setLayoutCount = set_layouts.size();
		create_info.pSetLayouts = set_layouts.begin();
		create_info.pushConstantRangeCount = 0;
		create_info.pPushConstantRanges = nullptr;

		CHECK(vkCreatePipelineLayout(_device._handle, &create_info, nullptr, &_handle));
	}

	VK_PipelineLayout::~VK_PipelineLayout() noexcept
	{
		vkDestroyPipelineLayout(_device._handle, _handle, nullptr);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Pipeline::~VK_Pipeline() noexcept
	{
		if (_handle != VK_NULL_HANDLE)
			vkDestroyPipeline(_device._handle, _handle, nullptr);
	}

	void VK_Pipeline::create(const VK_PipelineLayout& layout, VkRenderPass render_pass, VkShaderModule vertex_shader, VkShaderModule fragment_shader)
	{
		std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {};
		shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stages[0].pNext = nullptr;
		shader_stages[0].flags = 0;
		shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shader_stages[0].module = vertex_shader;
		shader_stages[0].pName = "main";
		shader_stages[0].pSpecializationInfo = nullptr;
		shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stages[1].pNext = nullptr;
		shader_stages[1].flags = 0;
		shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_stages[1].module = fragment_shader;
		shader_stages[1].pName = "main";
		shader_stages[1].pSpecializationInfo = nullptr;

		std::array<VkVertexInputBindingDescription, 1> vertex_input_bindings = {};
		vertex_input_bindings[0].binding = 0;
		vertex_input_bindings[0].stride = 32;
		vertex_input_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::array<VkVertexInputAttributeDescription, 2> vertex_input_attributes = {};
		vertex_input_attributes[0].location = 0;
		vertex_input_attributes[0].binding = 0;
		vertex_input_attributes[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertex_input_attributes[0].offset = 0;
		vertex_input_attributes[1].location = 1;
		vertex_input_attributes[1].binding = 0;
		vertex_input_attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertex_input_attributes[1].offset = 16;

		VkPipelineVertexInputStateCreateInfo vertex_input_state = {};
		vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_state.pNext = nullptr;
		vertex_input_state.flags = 0;
		vertex_input_state.vertexBindingDescriptionCount = vertex_input_bindings.size();
		vertex_input_state.pVertexBindingDescriptions = vertex_input_bindings.data();
		vertex_input_state.vertexAttributeDescriptionCount = vertex_input_attributes.size();
		vertex_input_state.pVertexAttributeDescriptions = vertex_input_attributes.data();

		VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {};
		input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly_state.pNext = nullptr;
		input_assembly_state.flags = 0;
		input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		input_assembly_state.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.pNext = nullptr;
		viewport_state.flags = 0;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = nullptr;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = nullptr;

		VkPipelineRasterizationStateCreateInfo rasterization_state = {};
		rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_state.pNext = nullptr;
		rasterization_state.flags = 0;
		rasterization_state.depthClampEnable = VK_TRUE;
		rasterization_state.rasterizerDiscardEnable = VK_FALSE;
		rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization_state.depthBiasEnable = VK_FALSE;
		rasterization_state.depthBiasConstantFactor = 0.f;
		rasterization_state.depthBiasClamp = 0.f;
		rasterization_state.depthBiasSlopeFactor = 0.f;
		rasterization_state.lineWidth = 1.f;

		VkPipelineMultisampleStateCreateInfo multisample_state = {};
		multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_state.pNext = 0;
		multisample_state.flags = 0;
		multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_state.sampleShadingEnable = VK_FALSE;
		multisample_state.minSampleShading = 0.f;
		multisample_state.pSampleMask = nullptr;
		multisample_state.alphaToCoverageEnable = VK_FALSE;
		multisample_state.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
		depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil_state.pNext = nullptr;
		depth_stencil_state.flags = 0;
		depth_stencil_state.depthTestEnable = VK_TRUE;
		depth_stencil_state.depthWriteEnable = VK_TRUE;
		depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
		depth_stencil_state.stencilTestEnable = VK_FALSE;
		depth_stencil_state.front.failOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.passOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.depthFailOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.compareOp = VK_COMPARE_OP_ALWAYS;
		depth_stencil_state.front.compareMask = 0;
		depth_stencil_state.front.writeMask = 0;
		depth_stencil_state.front.reference = 0;
		depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.back.depthFailOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depth_stencil_state.back.compareMask = 0;
		depth_stencil_state.back.writeMask = 0;
		depth_stencil_state.back.reference = 0;
		depth_stencil_state.minDepthBounds = 0;
		depth_stencil_state.maxDepthBounds = 0;

		VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
		color_blend_attachment_state.blendEnable = VK_FALSE;
		color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo color_blend_state = {};
		color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_state.pNext = nullptr;
		color_blend_state.flags = 0;
		color_blend_state.logicOpEnable = VK_FALSE;
		color_blend_state.logicOp = VK_LOGIC_OP_NO_OP;
		color_blend_state.attachmentCount = 1;
		color_blend_state.pAttachments = &color_blend_attachment_state;
		color_blend_state.blendConstants[0] = 1.f;
		color_blend_state.blendConstants[1] = 1.f;
		color_blend_state.blendConstants[2] = 1.f;
		color_blend_state.blendConstants[3] = 1.f;

		const std::array<VkDynamicState, 2> dynamic_state_data{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

		VkPipelineDynamicStateCreateInfo dynamic_state = {};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.pNext = nullptr;
		dynamic_state.flags = 0;
		dynamic_state.dynamicStateCount = dynamic_state_data.size();
		dynamic_state.pDynamicStates = dynamic_state_data.data();

		VkGraphicsPipelineCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.stageCount = shader_stages.size();
		create_info.pStages = shader_stages.data();
		create_info.pVertexInputState = &vertex_input_state;
		create_info.pInputAssemblyState = &input_assembly_state;
		create_info.pTessellationState = nullptr;
		create_info.pViewportState = &viewport_state;
		create_info.pRasterizationState = &rasterization_state;
		create_info.pMultisampleState = &multisample_state;
		create_info.pDepthStencilState = &depth_stencil_state;
		create_info.pColorBlendState = &color_blend_state;
		create_info.pDynamicState = &dynamic_state;
		create_info.layout = layout._handle;
		create_info.renderPass = render_pass;
		create_info.subpass = 0;
		create_info.basePipelineHandle = VK_NULL_HANDLE;
		create_info.basePipelineIndex = 0;

		CHECK(vkCreateGraphicsPipelines(_device._handle, VK_NULL_HANDLE, 1, &create_info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_CommandPool::VK_CommandPool(const VK_Device& device, uint32_t queue_family_index)
		: _device{device}
	{
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.queueFamilyIndex = queue_family_index;

		CHECK(vkCreateCommandPool(_device._handle, &create_info, nullptr, &_handle));
	}

	VK_CommandPool::~VK_CommandPool() noexcept
	{
		vkDestroyCommandPool(_device._handle, _handle, nullptr);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_CommandBuffer::VK_CommandBuffer(const VK_CommandPool& pool)
		: _pool{pool}
	{
		VkCommandBufferAllocateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		create_info.pNext = nullptr;
		create_info.commandPool = _pool._handle;
		create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		create_info.commandBufferCount = 1;

		CHECK(vkAllocateCommandBuffers(_pool._device._handle, &create_info, &_handle));
	}

	VK_CommandBuffer::~VK_CommandBuffer() noexcept
	{
		vkFreeCommandBuffers(_pool._device._handle, _pool._handle, 1, &_handle);
	}

	void VK_CommandBuffer::begin() const
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.pNext = nullptr;
		begin_info.flags = 0;
		begin_info.pInheritanceInfo = nullptr;

		CHECK(vkBeginCommandBuffer(_handle, &begin_info));
	}

	void VK_CommandBuffer::end() const
	{
		CHECK(vkEndCommandBuffer(_handle));
	}

	void VK_CommandBuffer::submit(VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) const
	{
		const VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &wait_semaphore;
		submit_info.pWaitDstStageMask = &pipeline_stage_flags;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_handle;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &signal_semaphore;

		CHECK(vkQueueSubmit(_pool._device._graphics_queue, 1, &submit_info, VK_NULL_HANDLE));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanSwapchain::VulkanSwapchain(const VK_Device& device, const VK_CommandPool& command_pool, const VK_PipelineLayout& pipeline_layout, VkShaderModule vertex_shader, VkShaderModule fragment_shader)
		: _device{device}
		, _swapchain{_device}
		, _depth_buffer{_device}
		, _render_pass{_device}
		, _framebuffers{_device}
		, _pipeline{_device}
		, _image_acquired{_device}
		, _rendering_complete{_device}
		, _command_buffer{command_pool}
	{
		_swapchain.create();
		_swapchain.create_views();

		_depth_buffer.create_image(VK_FORMAT_D16_UNORM);
		_depth_buffer.allocate_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		_depth_buffer.create_view();

		_render_pass.create(_swapchain, _depth_buffer);

		_framebuffers.create(_render_pass, _swapchain, _depth_buffer);

		_pipeline.create(pipeline_layout, _render_pass._handle, vertex_shader, fragment_shader);
	}

	void VulkanSwapchain::render(const std::function<void(VkCommandBuffer, const std::function<void(const std::function<void()>&)>&)>& callback) const
	{
		const auto framebuffer_index = _swapchain.acquire_next_image(_image_acquired._handle);
		_command_buffer.begin();
		callback(_command_buffer._handle, [this, framebuffer_index](const std::function<void()>& render_pass_callback)
		{
			std::array<VkClearValue, 2> clear_values;
			clear_values[0].color.float32[0] = 1.f / 32;
			clear_values[0].color.float32[1] = 1.f / 32;
			clear_values[0].color.float32[2] = 1.f / 32;
			clear_values[0].color.float32[3] = 1.f;
			clear_values[1].depthStencil.depth = 1.f;
			clear_values[1].depthStencil.stencil = 0;

			VkRenderPassBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			begin_info.pNext = nullptr;
			begin_info.renderPass = _render_pass._handle;
			begin_info.framebuffer = _framebuffers._handles[framebuffer_index];
			begin_info.renderArea.offset.x = 0;
			begin_info.renderArea.offset.y = 0;
			begin_info.renderArea.extent = _device._physical_device._surface_capabilities.currentExtent; // TODO: Use actual current extent.
			begin_info.clearValueCount = clear_values.size();
			begin_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(_command_buffer._handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(_command_buffer._handle, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline._handle);
			render_pass_callback();
			vkCmdEndRenderPass(_command_buffer._handle);
		});
		_command_buffer.end();
		_command_buffer.submit(_image_acquired._handle, _rendering_complete._handle);
		_swapchain.present(framebuffer_index, _rendering_complete._handle);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VulkanContext::VulkanContext(const WindowBackend& window)
		: _instance{}
		, _surface{_instance, window}
		, _physical_device{_surface}
		, _device{_physical_device}
		, _uniform_buffer{_device, 2 * sizeof(Matrix4)}
		, _vertex_buffer{_device, 1024}
		, _command_pool{_device, _physical_device._queue_family_index}
		, _descriptor_set_layout{_device, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}}}
		, _pipeline_layout{_device, {_descriptor_set_layout._handle}}
	{
		_uniform_buffer.create(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		_uniform_buffer.allocate_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		_vertex_buffer.create(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		_vertex_buffer.allocate_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		{
			VkDescriptorPoolSize dps = {};
			dps.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			dps.descriptorCount = 1;

			VkDescriptorPoolCreateInfo descriptor_pool_ci = {};
			descriptor_pool_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptor_pool_ci.pNext = nullptr;
			descriptor_pool_ci.flags = 0;
			descriptor_pool_ci.maxSets = 1;
			descriptor_pool_ci.poolSizeCount = 1;
			descriptor_pool_ci.pPoolSizes = &dps;

			CHECK(vkCreateDescriptorPool(_device._handle, &descriptor_pool_ci, nullptr, &_descriptor_pool));
		}

		{
			VkDescriptorSetAllocateInfo descriptor_set_ai = {};
			descriptor_set_ai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptor_set_ai.pNext = nullptr;
			descriptor_set_ai.descriptorPool = _descriptor_pool;
			descriptor_set_ai.descriptorSetCount = 1;
			descriptor_set_ai.pSetLayouts = &_descriptor_set_layout._handle;

			CHECK(vkAllocateDescriptorSets(_device._handle, &descriptor_set_ai, &_descriptor_set));
		}

		{
			const auto dbi = _uniform_buffer.descriptor_buffer_info();

			VkWriteDescriptorSet wds = {};
			wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			wds.pNext = nullptr;
			wds.dstSet = _descriptor_set;
			wds.dstBinding = 0;
			wds.dstArrayElement = 0;
			wds.descriptorCount = 1;
			wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			wds.pImageInfo = nullptr;
			wds.pBufferInfo = &dbi;
			wds.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(_device._handle, 1, &wds, 0, nullptr);
		}

		_vertex_shader = ::create_glsl_shader(_device._handle, VK_SHADER_STAGE_VERTEX_BIT,
			"#version 400\n"
			"#extension GL_ARB_separate_shader_objects : enable\n"
			"#extension GL_ARB_shading_language_420pack : enable\n"
			"layout (std140, binding = 0) uniform buffer\n"
			"{\n"
			"  mat4 mvp;\n"
			"} u_buffer;\n"
			"layout (location = 0) in vec4 i_position;\n"
			"layout (location = 1) in vec4 i_color;\n"
			"layout (location = 0) out vec4 o_color;\n"
			"void main()\n"
			"{\n"
			"  o_color = i_color;\n"
			"  gl_Position = u_buffer.mvp * i_position;\n"
			"}\n");

		_fragment_shader = ::create_glsl_shader(_device._handle, VK_SHADER_STAGE_FRAGMENT_BIT,
			"#version 400\n"
			"#extension GL_ARB_separate_shader_objects : enable\n"
			"#extension GL_ARB_shading_language_420pack : enable\n"
			"layout (location = 0) in vec4 i_color;\n"
			"layout (location = 0) out vec4 o_color;\n"
			"void main()\n"
			"{\n"
			"  o_color = i_color;\n"
			"}\n");
	}

	VulkanContext::~VulkanContext() noexcept
	{
		_swapchain.reset();
		if (_fragment_shader != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(_device._handle, _fragment_shader, nullptr);
			_fragment_shader = VK_NULL_HANDLE;
		}
		if (_vertex_shader != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(_device._handle, _vertex_shader, nullptr);
			_vertex_shader = VK_NULL_HANDLE;
		}
		if (_descriptor_set != VK_NULL_HANDLE)
		{
			std::ignore = vkFreeDescriptorSets(_device._handle, _descriptor_pool, 1, &_descriptor_set);
			_descriptor_set = VK_NULL_HANDLE;
		}
		if (_descriptor_pool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(_device._handle, _descriptor_pool, nullptr);
			_descriptor_pool = VK_NULL_HANDLE;
		}
	}

	void VulkanContext::render()
	{
		_device.wait_idle();
		if (!_swapchain)
			_swapchain = std::make_unique<VulkanSwapchain>(_device, _command_pool, _pipeline_layout, _vertex_shader, _fragment_shader);
		try
		{
			_swapchain->render([this](VkCommandBuffer command_buffer, const std::function<void(const std::function<void()>&)>& render_pass)
			{
				render_pass([this, command_buffer]
				{
					vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout._handle, 0, 1, &_descriptor_set, 0, nullptr);

					const std::array<VkDeviceSize, 1> vertex_buffer_offsets{0};
					vkCmdBindVertexBuffers(command_buffer, 0, 1, &_vertex_buffer._handle, vertex_buffer_offsets.data());

					// TODO: Do actual rendering.
				});
			});
		}
		catch (const VK_Swapchain::OutOfDate&)
		{
			_device.wait_idle();
			_swapchain.reset();
		}
	}
}
