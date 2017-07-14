#include "context.h"

#include <yttrium/math/matrix.h>
#include "../../system/window.h"

#include <cassert>
#include <cstring>
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
		throw std::runtime_error{function.substr(0, function.find('(')) + " = " + result_to_string()};
	}
}

#define CHECK(call) if (const auto result = (call)) vulkan_throw(result, #call);

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
		CHECK(vkCreateInstance(&instance_ci, nullptr, &_instance));

#ifdef VK_USE_PLATFORM_XCB_KHR
		VkXcbSurfaceCreateInfoKHR surface_ci = {};
		surface_ci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
		surface_ci.connection = window.xcb_connection();
		surface_ci.window = window.xcb_window();
		CHECK(vkCreateXcbSurfaceKHR(_instance, &surface_ci, nullptr, &_surface));
#endif

		uint32_t physical_device_count = 0;
		CHECK(vkEnumeratePhysicalDevices(_instance, &physical_device_count, nullptr));

		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		CHECK(vkEnumeratePhysicalDevices(_instance, &physical_device_count, physical_devices.data()));

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
				CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, _surface, &has_present_support));

				if (!has_present_support)
					continue;

				_physical_device = physical_device;
				device_queue_ci.queueFamilyIndex = i; // TODO: Support separate queues for graphics and present.
				break;
			}
		}

		if (_physical_device == VK_NULL_HANDLE)
			throw std::runtime_error{"No suitable physical device found"};

		vkGetPhysicalDeviceMemoryProperties(_physical_device, &_gpu_memory_props);

		VkSurfaceCapabilitiesKHR surface_caps = {};
		CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface, &surface_caps));

		if (surface_caps.currentExtent.width == 0xffffffff && surface_caps.currentExtent.height == 0xffffffff)
			throw std::runtime_error{"Bad surface size"};

		uint32_t surface_format_count = 0;
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface, &surface_format_count, nullptr));

		std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
		CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface, &surface_format_count, surface_formats.data()));

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
		CHECK(vkCreateDevice(_physical_device, &device_ci, nullptr, &_device));

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
		CHECK(vkCreateSwapchainKHR(_device, &swapchain_ci, nullptr, &_swapchain));

		uint32_t swapchain_image_count = 0;
		CHECK(vkGetSwapchainImagesKHR(_device, _swapchain, &swapchain_image_count, nullptr));

		std::vector<VkImage> swapchain_images(swapchain_image_count);
		CHECK(vkGetSwapchainImagesKHR(_device, _swapchain, &swapchain_image_count, swapchain_images.data()));

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
			CHECK(vkCreateImageView(_device, &image_view_ci, nullptr, &_image_views[i]));
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
		CHECK(vkCreateImage(_device, &depth_image_ci, nullptr, &_depth_image));

		VkMemoryRequirements depth_buffer_mr = {};
		vkGetImageMemoryRequirements(_device, _depth_image, &depth_buffer_mr);
		_depth_memory = allocate_memory(depth_buffer_mr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		CHECK(vkBindImageMemory(_device, _depth_image, _depth_memory, 0));

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
		CHECK(vkCreateImageView(_device, &depth_image_view_ci, nullptr, &_depth_image_view));

		VkBufferCreateInfo uniform_buffer_ci = {};
		uniform_buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		uniform_buffer_ci.pNext = nullptr;
		uniform_buffer_ci.flags = 0;
		uniform_buffer_ci.size = 2 * sizeof(Matrix4);
		uniform_buffer_ci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		uniform_buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		uniform_buffer_ci.queueFamilyIndexCount = 0;
		uniform_buffer_ci.pQueueFamilyIndices = nullptr;
		CHECK(vkCreateBuffer(_device, &uniform_buffer_ci, nullptr, &_uniform_buffer));

		VkMemoryRequirements uniform_buffer_mr = {};
		vkGetBufferMemoryRequirements(_device, _uniform_buffer, &uniform_buffer_mr);
		_uniform_buffer_memory = allocate_memory(uniform_buffer_mr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		CHECK(vkBindBufferMemory(_device, _uniform_buffer, _uniform_buffer_memory, 0));

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

		VkPipelineLayoutCreateInfo pipeline_layout_ci = {};
		pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_ci.pNext = nullptr;
		pipeline_layout_ci.flags = 0;
		pipeline_layout_ci.setLayoutCount = 1;
		pipeline_layout_ci.pSetLayouts = &_descriptor_set_layout;
		pipeline_layout_ci.pushConstantRangeCount = 0;
		pipeline_layout_ci.pPushConstantRanges = nullptr;
		CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_ci, nullptr, &_pipeline_layout));

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

		VkDescriptorSetAllocateInfo descriptor_set_ai = {};
		descriptor_set_ai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_ai.pNext = nullptr;
		descriptor_set_ai.descriptorPool = _descriptor_pool;
		descriptor_set_ai.descriptorSetCount = 1;
		descriptor_set_ai.pSetLayouts = &_descriptor_set_layout;
		CHECK(vkAllocateDescriptorSets(_device, &descriptor_set_ai, &_descriptor_set));

		VkDescriptorBufferInfo dbi = {};
		dbi.buffer = _uniform_buffer;
		dbi.offset = 0;
		dbi.range = uniform_buffer_ci.size;

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

		std::array<VkAttachmentDescription, 2> attachments = {};
		attachments[0].flags = 0;
		attachments[0].format = swapchain_ci.imageFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachments[1].flags = 0;
		attachments[1].format = depth_image_ci.format;
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

		VkCommandPoolCreateInfo command_pool_ci = {};
		command_pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_ci.pNext = nullptr;
		command_pool_ci.flags = 0;
		command_pool_ci.queueFamilyIndex = device_queue_ci.queueFamilyIndex;
		CHECK(vkCreateCommandPool(_device, &command_pool_ci, nullptr, &_command_pool));

		VkCommandBufferAllocateInfo command_buffer_ai = {};
		command_buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_ai.pNext = nullptr;
		command_buffer_ai.commandPool = _command_pool;
		command_buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_ai.commandBufferCount = 1;
		CHECK(vkAllocateCommandBuffers(_device, &command_buffer_ai, &_command_buffer));
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
