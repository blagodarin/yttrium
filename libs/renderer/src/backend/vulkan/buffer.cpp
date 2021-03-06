// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "buffer.h"

#include "context.h"
#include "handles.h"

#include <cassert>
#include <cstring>

namespace Yt
{
	VulkanBuffer::VulkanBuffer(const VulkanContext& context, uint32_t size, VkBufferUsageFlags buffer_usage, VkFlags memory_flags)
		: _context{ context }
		, _size{ size }
	{
		VkBufferCreateInfo buffer_info;
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.pNext = nullptr;
		buffer_info.flags = 0;
		buffer_info.size = _size;
		buffer_info.usage = buffer_usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buffer_info.queueFamilyIndexCount = 0;
		buffer_info.pQueueFamilyIndices = nullptr;

		VK_HBuffer buffer{ _context->_device };
		buffer.create(buffer_info);

		const auto memory_requirements = buffer.memory_requirements();

		VkMemoryAllocateInfo memory_info;
		memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_info.pNext = nullptr;
		memory_info.allocationSize = memory_requirements.size;
		memory_info.memoryTypeIndex = _context.memory_type_index(memory_requirements.memoryTypeBits, memory_flags);

		VK_HDeviceMemory memory{ _context->_device };
		memory.allocate(memory_info);

		buffer.bind_memory(memory.get());

		_handle = buffer.release();
		_memory = memory.release();
	}

	VulkanBuffer::~VulkanBuffer() noexcept
	{
		vkDestroyBuffer(_context->_device, _handle, nullptr);
		vkFreeMemory(_context->_device, _memory, nullptr);
	}

	VkDescriptorBufferInfo VulkanBuffer::descriptor_buffer_info() const noexcept
	{
		VkDescriptorBufferInfo result;
		result.buffer = _handle;
		result.offset = 0;
		result.range = _size;
		return result;
	}

	void VulkanBuffer::write(const void* data, size_t size, size_t offset)
	{
		assert(offset <= _size && size <= _size - offset);
		void* mapped_memory = nullptr;
		Y_VK_CHECK(vkMapMemory(_context->_device, _memory, offset, size, 0, &mapped_memory));
		std::memcpy(mapped_memory, data, size);
		vkUnmapMemory(_context->_device, _memory);
	}
}
