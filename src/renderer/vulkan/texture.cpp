#include "texture.h"

#include "renderer.h"

namespace Yttrium
{
	VulkanTexture2D::VulkanTexture2D(RendererImpl& renderer, VulkanContext& context, const ImageFormat& format, bool has_mipmaps, VkFormat vk_format, const void* data)
		: BackendTexture2D{renderer, format, has_mipmaps}
		, _image{context.create_texture_2d_image(format.width(), format.height(), vk_format)}
		, _sampler{context.create_texture_2d_sampler()}
	{
		_memory = context.allocate_memory(_image.memory_requirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		_image.bind_memory(_memory.get());

		_view = context.create_texture_2d_view(_image.get(), vk_format); // Must be created after memory binding.

		VK_Buffer staging{context.device(), static_cast<uint32_t>(format.frame_size()), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
		staging.write(data, format.frame_size());

		const auto command_buffer = context.allocate_command_buffer();
		command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		command_buffer.add_image_layout_transition(_image.get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		{
			VkBufferImageCopy region;
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset.x = 0;
			region.imageOffset.y = 0;
			region.imageOffset.z = 0;
			region.imageExtent.width = format.width();
			region.imageExtent.height = format.height();
			region.imageExtent.depth = 1;
			vkCmdCopyBufferToImage(command_buffer._handle, staging._handle, _image.get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		}
		command_buffer.add_image_layout_transition(_image.get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		command_buffer.end();
		command_buffer.submit_and_wait();
	}

	VkDescriptorImageInfo VulkanTexture2D::descriptor_image_info() const noexcept
	{
		VkDescriptorImageInfo info;
		info.sampler = _sampler.get();
		info.imageView = _view.get();
		info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		return info;
	}
}
