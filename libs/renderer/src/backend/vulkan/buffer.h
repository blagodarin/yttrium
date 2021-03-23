// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vulkan/vulkan.h>

namespace Yt
{
	class VulkanContext;

	class VulkanBuffer
	{
	public:
		VulkanBuffer(const VulkanContext&, uint32_t size, VkBufferUsageFlags buffer_usage, VkFlags memory_flags);
		~VulkanBuffer() noexcept;

		VkDescriptorBufferInfo descriptor_buffer_info() const noexcept;
		const VkBuffer& get() const noexcept { return _handle; }
		void write(const void* data, size_t size, size_t offset = 0);

	private:
		const VulkanContext& _context;
		const uint32_t _size;
		VkBuffer _handle = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
	};
}
