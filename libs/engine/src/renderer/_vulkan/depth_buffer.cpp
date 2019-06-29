//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "depth_buffer.h"

#include "context.h"
#include "handles.h"

namespace Yttrium
{
	VulkanDepthBuffer::VulkanDepthBuffer(const VulkanContext& context, uint32_t width, uint32_t height, VkFormat format)
		: _context{ context }
		, _format{ format }
	{
		VK_HDeviceMemory memory{ _context->_device };
		VK_HImage image{ _context->_device };
		VK_HImageView view{ _context->_device };

		VkImageCreateInfo image_info;
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.pNext = nullptr;
		image_info.flags = 0;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.format = _format;
		image_info.extent.width = width;
		image_info.extent.height = height;
		image_info.extent.depth = 1;
		image_info.mipLevels = 1;
		image_info.arrayLayers = 1;
		image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		image_info.tiling = _context.tiling(_format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
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
		memory_info.memoryTypeIndex = _context.memory_type_index(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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

	VulkanDepthBuffer::~VulkanDepthBuffer() noexcept
	{
		vkDestroyImageView(_context->_device, _view, nullptr);
		vkDestroyImage(_context->_device, _image, nullptr);
		vkFreeMemory(_context->_device, _memory, nullptr);
	}

	VkAttachmentDescription VulkanDepthBuffer::attachment_description() const noexcept
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
}
