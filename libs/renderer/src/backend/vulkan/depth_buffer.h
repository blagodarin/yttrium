// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vulkan/vulkan.h>

namespace Yt
{
	class VulkanContext;

	class VulkanDepthBuffer
	{
	public:
		VulkanDepthBuffer(const VulkanContext&, uint32_t width, uint32_t height, VkFormat);
		~VulkanDepthBuffer() noexcept;

		VkAttachmentDescription attachment_description() const noexcept;
		VkImageView view() const noexcept { return _view; }

	private:
		const VulkanContext& _context;
		const VkFormat _format;
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;
	};
}
