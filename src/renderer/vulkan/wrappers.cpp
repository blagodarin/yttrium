#include "wrappers.h"

#include "../../system/window.h"
#include "helpers.h"

#include <cassert>
#include <cstring>

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

#define CHECK(call) if (const auto result = (call)) ::vulkan_throw(result, #call);

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

	class VK_VkBuffer : public VK_Handle<VkBuffer>
	{
	public:
		VK_VkBuffer(VkDevice device) noexcept : _device{device} {}
		~VK_VkBuffer() noexcept { if (*this) vkDestroyBuffer(_device, _handle, nullptr); }

		void bind_memory(VkDeviceMemory memory) const
		{
			assert(*this && VK_NULL_HANDLE != memory);
			CHECK(vkBindBufferMemory(_device, _handle, memory, 0));
		}

		void create(const VkBufferCreateInfo& info)
		{
			assert(!*this);
			CHECK(vkCreateBuffer(_device, &info, nullptr, &_handle));
		}

		VkMemoryRequirements memory_requirements() const noexcept
		{
			assert(*this);
			VkMemoryRequirements requirements;
			vkGetBufferMemoryRequirements(_device, _handle, &requirements);
			return requirements;
		}

	private:
		const VkDevice _device;
	};

	class VK_VkDeviceMemory : public VK_Handle<VkDeviceMemory>
	{
	public:
		VK_VkDeviceMemory(VkDevice device) noexcept : _device{device} {}
		~VK_VkDeviceMemory() noexcept { if (*this) vkFreeMemory(_device, _handle, nullptr); }

		void allocate(const VkMemoryAllocateInfo& info)
		{
			assert(!*this);
			CHECK(vkAllocateMemory(_device, &info, nullptr, &_handle));
		}

	private:
		const VkDevice _device;
	};

	class VK_VkImage : public VK_Handle<VkImage>
	{
	public:
		VK_VkImage(VkDevice device) noexcept : _device{device} {}
		~VK_VkImage() noexcept { if (*this) vkDestroyImage(_device, _handle, nullptr); }

		void bind_memory(VkDeviceMemory memory) const
		{
			assert(*this && VK_NULL_HANDLE != memory);
			CHECK(vkBindImageMemory(_device, _handle, memory, 0));
		}

		void create(const VkImageCreateInfo& info)
		{
			assert(!*this);
			CHECK(vkCreateImage(_device, &info, nullptr, &_handle));
		}

		VkMemoryRequirements memory_requirements() const noexcept
		{
			assert(*this);
			VkMemoryRequirements requirements;
			vkGetImageMemoryRequirements(_device, _handle, &requirements);
			return requirements;
		}

	private:
		const VkDevice _device;
	};

	class VK_VkImageView : public VK_Handle<VkImageView>
	{
	public:
		VK_VkImageView(VkDevice device) noexcept : _device{device} {}
		VK_VkImageView(VK_VkImageView&& view) noexcept : VK_Handle{view._handle}, _device{view._device} { view._handle = VK_NULL_HANDLE; }
		~VK_VkImageView() noexcept { if (*this) vkDestroyImageView(_device, _handle, nullptr); }

		void create(const VkImageViewCreateInfo& info)
		{
			assert(!*this);
			CHECK(vkCreateImageView(_device, &info, nullptr, &_handle));
		}

	private:
		const VkDevice _device;
	};

	class VK_VkSwapchainKHR : public VK_Handle<VkSwapchainKHR>
	{
	public:
		VK_VkSwapchainKHR(VkDevice device) noexcept : _device{device} {}
		~VK_VkSwapchainKHR() noexcept { if (*this) vkDestroySwapchainKHR(_device, _handle, nullptr); }

		void create(const VkSwapchainCreateInfoKHR& info)
		{
			assert(!*this);
			CHECK(vkCreateSwapchainKHR(_device, &info, nullptr, &_handle));
		}

	private:
		const VkDevice _device;
	};
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

	void VK_Device::wait_idle() const
	{
		CHECK(vkDeviceWaitIdle(_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Swapchain::VK_Swapchain(const VK_Device& device)
		: _device{device}
	{
		const auto surface_formats = _device._physical_device.surface_formats();
		_format = surface_formats.empty() ? VK_FORMAT_B8G8R8A8_UNORM : surface_formats[0].format;

		VkSwapchainCreateInfoKHR swapchain_info;
		swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_info.pNext = nullptr;
		swapchain_info.flags = 0;
		swapchain_info.surface = _device._physical_device._surface._handle;
		swapchain_info.minImageCount = _device._physical_device._surface_capabilities.minImageCount;
		swapchain_info.imageFormat = _format;
		swapchain_info.imageColorSpace = surface_formats.empty() ? VK_COLOR_SPACE_SRGB_NONLINEAR_KHR : surface_formats[0].colorSpace;
		swapchain_info.imageExtent = _device._physical_device._surface_capabilities.currentExtent;
		swapchain_info.imageArrayLayers = 1;
		swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_info.queueFamilyIndexCount = 0;
		swapchain_info.pQueueFamilyIndices = nullptr;
		swapchain_info.preTransform = _device._physical_device.surface_transform();
		swapchain_info.compositeAlpha = _device._physical_device.composite_alpha();
		swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchain_info.clipped = VK_TRUE;
		swapchain_info.oldSwapchain = VK_NULL_HANDLE;

		VK_VkSwapchainKHR swapchain{_device._handle};
		swapchain.create(swapchain_info);

		uint32_t image_count = 0;
		CHECK(vkGetSwapchainImagesKHR(_device._handle, swapchain.get(), &image_count, nullptr));

		std::vector<VkImage> images(image_count, VK_NULL_HANDLE);
		CHECK(vkGetSwapchainImagesKHR(_device._handle, swapchain.get(), &image_count, images.data()));

		std::vector<VK_VkImageView> views;
		views.reserve(image_count);
		for (const auto image : images)
		{
			VkImageViewCreateInfo view_info;
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.pNext = nullptr;
			view_info.flags = 0;
			view_info.image = image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = _format;
			view_info.components.r = VK_COMPONENT_SWIZZLE_R;
			view_info.components.g = VK_COMPONENT_SWIZZLE_G;
			view_info.components.b = VK_COMPONENT_SWIZZLE_B;
			view_info.components.a = VK_COMPONENT_SWIZZLE_A;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;
			views.emplace_back(_device._handle).create(view_info);
		}

		_views.reserve(views.size());

		_handle = swapchain.release();
		for (auto& view : views)
			_views.emplace_back(view.release());
	}

	VK_Swapchain::~VK_Swapchain() noexcept
	{
		for (const auto view : _views)
			vkDestroyImageView(_device._handle, view, nullptr);
		vkDestroySwapchainKHR(_device._handle, _handle, nullptr);
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

	VK_DepthBuffer::VK_DepthBuffer(const VK_Device& device, VkFormat format, VkFlags memory_flags)
		: _device{device}
		, _format{format}
	{
		VK_VkDeviceMemory memory{_device._handle};
		VK_VkImage image{_device._handle};
		VK_VkImageView view{_device._handle};

		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.pNext = nullptr;
		image_info.flags = 0;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = _format;
		image_info.extent.width = _device._physical_device._surface_capabilities.currentExtent.width;
		image_info.extent.height = _device._physical_device._surface_capabilities.currentExtent.height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = _device._physical_device.tiling(_format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		image_info.queueFamilyIndexCount = 0;
		image_info.pQueueFamilyIndices = nullptr;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image.create(image_info);

		const auto memory_requirements = image.memory_requirements();

		VkMemoryAllocateInfo memory_info;
		memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_info.pNext = nullptr;
		memory_info.allocationSize = memory_requirements.size;
		memory_info.memoryTypeIndex = _device._physical_device.memory_type_index(memory_requirements.memoryTypeBits, memory_flags);
		memory.allocate(memory_info);

		image.bind_memory(memory.get());

		VkImageViewCreateInfo view_info;
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.pNext = nullptr;
		view_info.flags = 0;
		view_info.image = image.get();
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = _format;
		view_info.components.r = VK_COMPONENT_SWIZZLE_R;
		view_info.components.g = VK_COMPONENT_SWIZZLE_G;
		view_info.components.b = VK_COMPONENT_SWIZZLE_B;
		view_info.components.a = VK_COMPONENT_SWIZZLE_A;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		view.create(view_info);

		_image = image.release();
		_memory = memory.release();
		_view = view.release();
	}

	VK_DepthBuffer::~VK_DepthBuffer() noexcept
	{
		vkDestroyImageView(_device._handle, _view, nullptr);
		vkDestroyImage(_device._handle, _image, nullptr);
		vkFreeMemory(_device._handle, _memory, nullptr);
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

	VK_Buffer::VK_Buffer(const VK_Device& device, uint32_t size, VkBufferUsageFlags buffer_usage, VkFlags memory_flags)
		: _device{device}
		, _size{size}
	{
		VkBufferCreateInfo create_info;
		create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.size = _size;
		create_info.usage = buffer_usage;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;

		VK_VkBuffer buffer{_device._handle};
		buffer.create(create_info);

		const auto memory_requirements = buffer.memory_requirements();

		VkMemoryAllocateInfo allocate_info;
		allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.pNext = nullptr;
		allocate_info.allocationSize = memory_requirements.size;
		allocate_info.memoryTypeIndex = _device._physical_device.memory_type_index(memory_requirements.memoryTypeBits, memory_flags);

		VK_VkDeviceMemory memory{_device._handle};
		memory.allocate(allocate_info);

		buffer.bind_memory(memory.get());

		_handle = buffer.release();
		_memory = memory.release();
	}

	VK_Buffer::~VK_Buffer() noexcept
	{
		vkDestroyBuffer(_device._handle, _handle, nullptr);
		vkFreeMemory(_device._handle, _memory, nullptr);
	}

	VkDescriptorBufferInfo VK_Buffer::descriptor_buffer_info() const noexcept
	{
		VkDescriptorBufferInfo result;
		result.buffer = _handle;
		result.offset = 0;
		result.range = _size;
		return result;
	}

	void VK_Buffer::write(const void* data, size_t size, size_t offset)
	{
		assert(offset <= _size && size <= _size - offset);
		void* mapped_memory = nullptr;
		CHECK(vkMapMemory(_device._handle, _memory, offset, size, 0, &mapped_memory));
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DescriptorPool::VK_DescriptorPool(const VK_Device& device, uint32_t max_sets, std::initializer_list<Size> sizes, uint32_t flags)
		: _device{device}
	{
		VkDescriptorPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = flags;
		create_info.maxSets = max_sets;
		create_info.poolSizeCount = sizes.size();
		create_info.pPoolSizes = sizes.begin();

		CHECK(vkCreateDescriptorPool(_device._handle, &create_info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DescriptorSet::VK_DescriptorSet(const VK_DescriptorPool& pool, VkDescriptorSetLayout layout)
		: _pool{pool}
	{
		VkDescriptorSetAllocateInfo allocate_info = {};
		allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocate_info.pNext = nullptr;
		allocate_info.descriptorPool = _pool._handle;
		allocate_info.descriptorSetCount = 1;
		allocate_info.pSetLayouts = &layout;

		CHECK(vkAllocateDescriptorSets(_pool._device._handle, &allocate_info, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_ShaderModule::VK_ShaderModule(const VK_Device& device, VkShaderStageFlagBits stage, const std::vector<uint32_t>& data)
		: _device{device}
		, _stage{stage}
	{
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.codeSize = data.size() * sizeof(uint32_t);
		create_info.pCode = data.data();

		CHECK(vkCreateShaderModule(_device._handle, &create_info, nullptr, &_handle));
	}

	std::vector<VkPipelineShaderStageCreateInfo> VK_ShaderModule::make_stages(std::initializer_list<const VK_ShaderModule*> modules)
	{
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		stages.reserve(modules.size());
		for (const auto module : modules)
		{
			auto& stage = stages.emplace_back();
			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.pNext = nullptr;
			stage.flags = 0;
			stage.stage = module->_stage;
			stage.module = module->_handle;
			stage.pName = "main";
			stage.pSpecializationInfo = nullptr;
		}
		return stages;
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Pipeline::~VK_Pipeline() noexcept
	{
		if (_handle != VK_NULL_HANDLE)
			vkDestroyPipeline(_device._handle, _handle, nullptr);
	}

	void VK_Pipeline::create(const VK_PipelineLayout& layout, VkRenderPass render_pass, const VulkanVertexFormat& vertex_format, const std::vector<VkPipelineShaderStageCreateInfo>& shader_stages)
	{
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
		create_info.pVertexInputState = &vertex_format._input;
		create_info.pInputAssemblyState = &vertex_format._assembly;
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
}
