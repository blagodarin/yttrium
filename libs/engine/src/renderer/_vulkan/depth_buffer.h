//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <vulkan/vulkan.h>

namespace Yttrium
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
