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

#pragma once

#include <functional>

#include "depth_buffer.h"
#include "wrappers.h"

namespace Yttrium
{
	class VulkanContext;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(const VulkanContext&, const VK_PipelineLayout&, const std::vector<VkPipelineShaderStageCreateInfo>&);
		~VulkanSwapchain() noexcept = default;

		void render(const std::function<void(VkCommandBuffer, const std::function<void(const std::function<void()>&)>&)>&) const; // TODO: Improve readability.

	private:
		void present(uint32_t framebuffer_index, VkSemaphore) const;

	public:
		const VulkanContext& _context;
		VK_Swapchain _swapchain;
		VulkanDepthBuffer _depth_buffer;
		VK_RenderPass _render_pass;
		VK_Framebuffers _framebuffers;
		VK_Pipeline _pipeline;
		VK_Semaphore _image_acquired;
		VK_Semaphore _rendering_complete;
		VK_CommandBuffer _command_buffer;
	};
}
