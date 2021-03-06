// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "texture.h"

#include "renderer.h"

namespace Yt
{
	VulkanTexture2D::VulkanTexture2D(RenderBackend& backend, VulkanContext& context, const ImageInfo& info, bool has_mipmaps, VkFormat vk_format, const void* data)
		: BackendTexture2D{ backend, info, has_mipmaps }
		, _image{ context.create_texture_2d_image(info.width(), info.height(), vk_format) }
		, _sampler{ context.create_texture_2d_sampler() }
	{
		_memory = context.allocate_memory(_image.memory_requirements(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		_image.bind_memory(_memory.get());

		_view = context.create_texture_2d_view(_image.get(), vk_format); // Must be created after memory binding.

		VulkanBuffer staging_buffer{ context, static_cast<std::uint32_t>(info.frame_size()), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
		staging_buffer.write(data, info.frame_size());

		const auto command_buffer = context.allocate_command_buffer();
		command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		command_buffer.add_image_layout_transition(_image.get(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		{
			VkBufferImageCopy region;
			region.bufferOffset = 0;
			region.bufferRowLength = static_cast<std::uint32_t>(info.stride() / ImageInfo::pixel_size(info.pixel_format()));
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset.x = 0;
			region.imageOffset.y = 0;
			region.imageOffset.z = 0;
			region.imageExtent.width = static_cast<std::uint32_t>(info.width());
			region.imageExtent.height = static_cast<std::uint32_t>(info.height());
			region.imageExtent.depth = 1;
			vkCmdCopyBufferToImage(command_buffer.get(), staging_buffer.get(), _image.get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
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
