#include "context.h"

#include <yttrium/math/matrix.h>
#include "../../system/window.h"

#include <cassert>
#include <cstring>
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
		throw std::runtime_error{function.substr(0, function.find('(')) + " = " + result_to_string()};
	}

#define CHECK(call) if (const auto result = (call)) vulkan_throw(result, #call);

	VkInstance create_instance()
	{
		static const auto extensions =
		{
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_XCB_KHR
			VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
		};

		VkApplicationInfo application_info = {};
		application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		application_info.pNext = nullptr;
		application_info.pApplicationName = nullptr;
		application_info.applicationVersion = 0;
		application_info.pEngineName = nullptr;
		application_info.engineVersion = 0;
		application_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.pApplicationInfo = &application_info;
		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = nullptr;
		create_info.enabledExtensionCount = extensions.size();
		create_info.ppEnabledExtensionNames = extensions.begin();

		VkInstance instance = VK_NULL_HANDLE;
		CHECK(vkCreateInstance(&create_info, nullptr, &instance));
		return instance;
	}

	VkSurfaceKHR create_surface(VkInstance instance, const Yttrium::WindowBackend& window)
	{
#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		create_info.connection = window.xcb_connection();
		create_info.window = window.xcb_window();

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		CHECK(vkCreateXcbSurfaceKHR(instance, &create_info, nullptr, &surface));
		return surface;
#endif
	}

	std::pair<VkPhysicalDevice, uint32_t> select_physical_device(VkInstance instance, VkSurfaceKHR surface)
	{
		uint32_t physical_device_count = 0;
		CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr));

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		CHECK(vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data()));

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
				CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &has_present_support));
				if (has_present_support)
					return {physical_device, i}; // TODO: Support separate queues for graphics and present.
			}
		}

		throw std::runtime_error{"No suitable physical device found"};
	}

	std::vector<VkSurfaceFormatKHR> get_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
	{
		uint32_t count = 0;
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr));
		std::vector<VkSurfaceFormatKHR> formats(count);
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, formats.data()));
		if (formats.empty())
			throw std::runtime_error{"No surface formats defined"};
		else if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
			formats.clear();
		return formats;
	}

	VkDevice create_device(VkPhysicalDevice physical_device, uint32_t queue_family_index)
	{
		const float queue_prioritiy = 0.f;

		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.pNext = nullptr;
		queue_create_info.flags = 0;
		queue_create_info.queueFamilyIndex = queue_family_index;
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

		VkDevice device = VK_NULL_HANDLE;
		CHECK(vkCreateDevice(physical_device, &create_info, nullptr, &device));
		return device;
	}

	std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
	{
		uint32_t count = 0;
		CHECK(vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr));
		std::vector<VkImage> images(count);
		CHECK(vkGetSwapchainImagesKHR(device, swapchain, &count, images.data()));
		return images;
	}

	VkImageView create_swapchain_view(VkDevice device, VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.image = image;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = format;
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
		CHECK(vkCreateImageView(device, &create_info, nullptr, &view));
		return view;
	}

	VkImage create_depth_image(VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.format = format;
		create_info.extent.width = width;
		create_info.extent.height = height;
		create_info.extent.depth = 1;
		create_info.mipLevels = 1;
		create_info.arrayLayers = 1;
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		create_info.tiling = tiling;
		create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
		create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkImage image = VK_NULL_HANDLE;
		CHECK(vkCreateImage(device, &create_info, nullptr, &image));
		return image;
	}

	VkImageView create_depth_view(VkDevice device, VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.image = image;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_R;
		create_info.components.g = VK_COMPONENT_SWIZZLE_G;
		create_info.components.b = VK_COMPONENT_SWIZZLE_B;
		create_info.components.a = VK_COMPONENT_SWIZZLE_A;
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		VkImageView view = VK_NULL_HANDLE;
		CHECK(vkCreateImageView(device, &create_info, nullptr, &view));
		return view;
	}

	VkBuffer create_uniform_buffer(VkDevice device, uint32_t size)
	{
		VkBufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.size = size;
		create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;

		VkBuffer buffer = VK_NULL_HANDLE;
		CHECK(vkCreateBuffer(device, &create_info, nullptr, &buffer));
		return buffer;
	}

	VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index)
	{
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.queueFamilyIndex = queue_family_index;

		VkCommandPool command_pool = VK_NULL_HANDLE;
		CHECK(vkCreateCommandPool(device, &create_info, nullptr, &command_pool));
		return command_pool;
	}

	VkCommandBuffer allocate_command_buffer(VkDevice device, VkCommandPool pool)
	{
		VkCommandBufferAllocateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		create_info.pNext = nullptr;
		create_info.commandPool = pool;
		create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		create_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer = VK_NULL_HANDLE;
		CHECK(vkAllocateCommandBuffers(device, &create_info, &command_buffer));
		return command_buffer;
	}
}

namespace Yttrium
{
	void VulkanContext::initialize(const WindowBackend& window)
	{
		reset();

		_instance = ::create_instance();
		_surface = ::create_surface(_instance, window);
		std::tie(_physical_device, _queue_family_index) = ::select_physical_device(_instance, _surface);
		vkGetPhysicalDeviceMemoryProperties(_physical_device, &_gpu_memory_props);

		const auto surface_formats = ::get_surface_formats(_physical_device, _surface);

		VkSurfaceCapabilitiesKHR surface_capabilities = {};
		CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &surface_capabilities));
		if (surface_capabilities.currentExtent.width == 0xffffffff && surface_capabilities.currentExtent.height == 0xffffffff)
			throw std::runtime_error{"Bad surface size"};

		_device = ::create_device(_physical_device, _queue_family_index);

		const auto swapchain_image_format = surface_formats.empty() ? VK_FORMAT_B8G8R8A8_UNORM : surface_formats[0].format;
		const auto swapchain_color_space = surface_formats.empty() ? VK_COLOR_SPACE_SRGB_NONLINEAR_KHR : surface_formats[0].colorSpace;

		{
			VkSwapchainCreateInfoKHR swapchain_ci = {};
			swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchain_ci.pNext = nullptr;
			swapchain_ci.flags = 0;
			swapchain_ci.surface = _surface;
			swapchain_ci.minImageCount = surface_capabilities.minImageCount;
			swapchain_ci.imageFormat = swapchain_image_format;
			swapchain_ci.imageColorSpace = swapchain_color_space;
			swapchain_ci.imageExtent = surface_capabilities.currentExtent;
			swapchain_ci.imageArrayLayers = 1;
			swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchain_ci.queueFamilyIndexCount = 0;
			swapchain_ci.pQueueFamilyIndices = nullptr;
			swapchain_ci.preTransform = surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surface_capabilities.currentTransform;
			swapchain_ci.compositeAlpha = [&surface_capabilities]
			{
				for (const auto bit : {VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR})
					if (surface_capabilities.supportedCompositeAlpha & static_cast<VkFlags>(bit))
						return bit;
				return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			}();
			swapchain_ci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
			swapchain_ci.clipped = VK_TRUE;
			swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
			CHECK(vkCreateSwapchainKHR(_device, &swapchain_ci, nullptr, &_swapchain));
		}

		for (const auto image : ::get_swapchain_images(_device, _swapchain))
			_image_views.emplace_back(::create_swapchain_view(_device, image, swapchain_image_format));

		const auto depth_image_format = VK_FORMAT_D16_UNORM;
		const auto depth_image_tiling = [this, depth_image_format]
		{
			VkFormatProperties depth_format_props = {};
			vkGetPhysicalDeviceFormatProperties(_physical_device, depth_image_format, &depth_format_props);
			if (depth_format_props.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return VK_IMAGE_TILING_LINEAR;
			else if (depth_format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				return VK_IMAGE_TILING_OPTIMAL;
			else
				throw std::runtime_error{"Depth buffer format is not supported"};
		}();

		_depth_image = ::create_depth_image(_device, surface_capabilities.currentExtent.width, surface_capabilities.currentExtent.height, depth_image_format, depth_image_tiling);

		VkMemoryRequirements depth_buffer_mr = {};
		vkGetImageMemoryRequirements(_device, _depth_image, &depth_buffer_mr);
		_depth_memory = allocate_memory(depth_buffer_mr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CHECK(vkBindImageMemory(_device, _depth_image, _depth_memory, 0));

		_depth_image_view = ::create_depth_view(_device, _depth_image, depth_image_format);

		const auto uniform_buffer_size = 2 * sizeof(Matrix4);
		_uniform_buffer = ::create_uniform_buffer(_device, uniform_buffer_size);

		VkMemoryRequirements uniform_buffer_mr = {};
		vkGetBufferMemoryRequirements(_device, _uniform_buffer, &uniform_buffer_mr);
		_uniform_buffer_memory = allocate_memory(uniform_buffer_mr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		CHECK(vkBindBufferMemory(_device, _uniform_buffer, _uniform_buffer_memory, 0));

		{
			VkDescriptorSetLayoutBinding dslb = {};
			dslb.binding = 0;
			dslb.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			dslb.descriptorCount = 1;
			dslb.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			dslb.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo descriptor_set_layout_ci = {};
			descriptor_set_layout_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptor_set_layout_ci.pNext = nullptr;
			descriptor_set_layout_ci.flags = 0;
			descriptor_set_layout_ci.bindingCount = 1;
			descriptor_set_layout_ci.pBindings = &dslb;
			CHECK(vkCreateDescriptorSetLayout(_device, &descriptor_set_layout_ci, nullptr, &_descriptor_set_layout));
		}

		{
			VkPipelineLayoutCreateInfo pipeline_layout_ci = {};
			pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipeline_layout_ci.pNext = nullptr;
			pipeline_layout_ci.flags = 0;
			pipeline_layout_ci.setLayoutCount = 1;
			pipeline_layout_ci.pSetLayouts = &_descriptor_set_layout;
			pipeline_layout_ci.pushConstantRangeCount = 0;
			pipeline_layout_ci.pPushConstantRanges = nullptr;
			CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_ci, nullptr, &_pipeline_layout));
		}

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
			CHECK(vkCreateDescriptorPool(_device, &descriptor_pool_ci, nullptr, &_descriptor_pool));
		}

		{
			VkDescriptorSetAllocateInfo descriptor_set_ai = {};
			descriptor_set_ai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptor_set_ai.pNext = nullptr;
			descriptor_set_ai.descriptorPool = _descriptor_pool;
			descriptor_set_ai.descriptorSetCount = 1;
			descriptor_set_ai.pSetLayouts = &_descriptor_set_layout;
			CHECK(vkAllocateDescriptorSets(_device, &descriptor_set_ai, &_descriptor_set));
		}

		{
			VkDescriptorBufferInfo dbi = {};
			dbi.buffer = _uniform_buffer;
			dbi.offset = 0;
			dbi.range = uniform_buffer_size;

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
			vkUpdateDescriptorSets(_device, 1, &wds, 0, nullptr);
		}

		{
			std::array<VkAttachmentDescription, 2> attachments = {};
			attachments[0].flags = 0;
			attachments[0].format = swapchain_image_format;
			attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			attachments[1].flags = 0;
			attachments[1].format = depth_image_format;
			attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
			attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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

			VkRenderPassCreateInfo render_pass_ci = {};
			render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			render_pass_ci.pNext = nullptr;
			render_pass_ci.flags = 0;
			render_pass_ci.attachmentCount = attachments.size();
			render_pass_ci.pAttachments = attachments.data();
			render_pass_ci.subpassCount = 1;
			render_pass_ci.pSubpasses = &subpass;
			render_pass_ci.dependencyCount = 0;
			render_pass_ci.pDependencies = nullptr;
			CHECK(vkCreateRenderPass(_device, &render_pass_ci, nullptr, &_render_pass));
		}

		_command_pool = ::create_command_pool(_device, _queue_family_index);
		_command_buffer = ::allocate_command_buffer(_device, _command_pool);
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
		if (_render_pass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(_device, _render_pass, nullptr);
			_render_pass = VK_NULL_HANDLE;
		}
		if (_descriptor_set != VK_NULL_HANDLE)
		{
			std::ignore = vkFreeDescriptorSets(_device, _descriptor_pool, 1, &_descriptor_set);
			_descriptor_set = VK_NULL_HANDLE;
		}
		if (_descriptor_pool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
			_descriptor_pool = VK_NULL_HANDLE;
		}
		if (_pipeline_layout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
			_pipeline_layout = VK_NULL_HANDLE;
		}
		if (_descriptor_set_layout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(_device, _descriptor_set_layout, nullptr);
			_descriptor_set_layout = VK_NULL_HANDLE;
		}
		if (_uniform_buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(_device, _uniform_buffer, nullptr);
			_uniform_buffer = VK_NULL_HANDLE;
		}
		if (_uniform_buffer_memory != VK_NULL_HANDLE)
		{
			vkFreeMemory(_device, _uniform_buffer_memory, nullptr);
			_uniform_buffer_memory = VK_NULL_HANDLE;
		}
		if (_depth_image_view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(_device, _depth_image_view, nullptr);
			_depth_image_view = VK_NULL_HANDLE;
		}
		if (_depth_image != VK_NULL_HANDLE)
		{
			vkDestroyImage(_device, _depth_image, nullptr);
			_depth_image = VK_NULL_HANDLE;
		}
		if (_depth_memory != VK_NULL_HANDLE)
		{
			vkFreeMemory(_device, _depth_memory, nullptr);
			_depth_memory = VK_NULL_HANDLE;
		}
		for (const auto view : _image_views)
			vkDestroyImageView(_device, view, nullptr);
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

	void VulkanContext::update_uniforms(const void* data, size_t size)
	{
		void* memory = nullptr;
		CHECK(vkMapMemory(_device, _uniform_buffer_memory, 0, size, 0, &memory));
		std::memcpy(memory, data, size);
		vkUnmapMemory(_device, _uniform_buffer_memory);
	}

	VkDeviceMemory VulkanContext::allocate_memory(const VkMemoryRequirements& requirements, uint32_t flags) const
	{
		assert(_device);
		VkMemoryAllocateInfo mai = {};
		mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mai.pNext = nullptr;
		mai.allocationSize = requirements.size;
		mai.memoryTypeIndex = find_memory_type(requirements.memoryTypeBits, flags);
		VkDeviceMemory handle = VK_NULL_HANDLE;
		CHECK(vkAllocateMemory(_device, &mai, nullptr, &handle));
		return handle;
	}

	uint32_t VulkanContext::find_memory_type(uint32_t type_bits, uint32_t property_bits) const
	{
		assert(_physical_device);
		for (uint32_t i = 0; i < _gpu_memory_props.memoryTypeCount; ++i)
			if (type_bits & (1u << i))
				if ((_gpu_memory_props.memoryTypes[i].propertyFlags & property_bits) == property_bits)
					return i;
		throw std::runtime_error{"No suitable memory type found"};
	}
}
