#include "context.h"

#include "handles.h"
#include "helpers.h"

namespace Yttrium
{
	VulkanSwapchain::VulkanSwapchain(const VK_Device& device, const VK_CommandPool& command_pool, const VK_PipelineLayout& pipeline_layout, const std::vector<VkPipelineShaderStageCreateInfo>& shader_stages)
		: _device{device}
		, _swapchain{_device}
		, _depth_buffer{_device, VK_FORMAT_D16_UNORM, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT}
		, _render_pass{_swapchain, _depth_buffer}
		, _framebuffers{_device}
		, _pipeline{_device}
		, _image_acquired{_device}
		, _rendering_complete{_device}
		, _command_buffer{command_pool}
	{
		_framebuffers.create(_render_pass, _swapchain, _depth_buffer);
		_pipeline.create(pipeline_layout, _render_pass._handle, VulkanVertexFormat{{VA::f4, VA::f4}}, shader_stages);
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

	VulkanContext::VulkanContext(const WindowBackend& window)
		: _instance{}
		, _surface{_instance, window}
		, _physical_device{_surface}
		, _device{_physical_device}
		, _command_pool{_device, _physical_device._queue_family_index}
	{
	}

	VK_HDeviceMemory VulkanContext::allocate_memory(const VkMemoryRequirements& requirements, VkMemoryPropertyFlags flags) const
	{
		VkMemoryAllocateInfo info;
		info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.allocationSize = requirements.size;
		info.memoryTypeIndex = _device._physical_device.memory_type_index(requirements.memoryTypeBits, flags);

		VK_HDeviceMemory memory{_device._handle};
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

		VK_HImage image{_device._handle};
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
		info.anisotropyEnable = _physical_device._features.samplerAnisotropy;
		info.maxAnisotropy = _physical_device._features.samplerAnisotropy ? 16.f : 1.f;
		info.compareEnable = VK_FALSE;
		info.compareOp = VK_COMPARE_OP_ALWAYS;
		info.minLod = 0.f;
		info.maxLod = 0.f;
		info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		info.unnormalizedCoordinates = VK_FALSE;

		VK_HSampler sampler{_device._handle};
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

		VK_HImageView view{_device._handle};
		view.create(info);
		return view;
	}
}
