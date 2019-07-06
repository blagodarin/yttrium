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

#include "swapchain.h"

#include "helpers.h"

namespace
{
	Yttrium::VK_RenderPass create_render_pass(VkDevice device, const Yttrium::VK_Swapchain& swapchain, const Yttrium::VulkanDepthBuffer& depth_buffer)
	{
		const std::array<VkAttachmentDescription, 2> attachment_descriptions{ swapchain.attachment_description(), depth_buffer.attachment_description() };

		VkAttachmentReference color_reference;
		color_reference.attachment = 0;
		color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_reference;
		depth_reference.attachment = 1;
		depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass;
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

		VkRenderPassCreateInfo create_info;
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.attachmentCount = attachment_descriptions.size();
		create_info.pAttachments = attachment_descriptions.data();
		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass;
		create_info.dependencyCount = 0;
		create_info.pDependencies = nullptr;

		Yttrium::VK_RenderPass render_pass{ device };
		render_pass.create(create_info);
		return render_pass;
	}
}

namespace Yttrium
{
	VulkanSwapchain::VulkanSwapchain(const VulkanContext& context, const VK_PipelineLayout& pipeline_layout, const std::vector<VkPipelineShaderStageCreateInfo>& shader_stages)
		: _context{ context }
		, _swapchain{ _context }
		, _depth_buffer{ _context, _context->_surface_capabilities.currentExtent.width, _context->_surface_capabilities.currentExtent.height, VK_FORMAT_D16_UNORM }
		, _render_pass{ ::create_render_pass(_context->_device, _swapchain, _depth_buffer) }
		, _framebuffers{ _context }
		, _pipeline{ _context }
		, _image_acquired{ _context }
		, _rendering_complete{ _context }
		, _command_buffer{ _context }
	{
		_framebuffers.create(_render_pass.get(), _swapchain, _depth_buffer.view());
		_pipeline.create(pipeline_layout, _render_pass.get(), VulkanVertexFormat{ { VA::f4, VA::f4 } }, shader_stages);
	}

	void VulkanSwapchain::render(const std::function<void(VkCommandBuffer, const std::function<void(const std::function<void()>&)>&)>& callback) const
	{
		const auto framebuffer_index = _swapchain.acquire_next_image(_image_acquired._handle);
		_command_buffer.begin();
		callback(_command_buffer.get(), [this, framebuffer_index](const std::function<void()>& render_pass_callback) {
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
			begin_info.renderPass = _render_pass.get();
			begin_info.framebuffer = _framebuffers._handles[framebuffer_index];
			begin_info.renderArea.offset.x = 0;
			begin_info.renderArea.offset.y = 0;
			begin_info.renderArea.extent = _context->_surface_capabilities.currentExtent; // TODO: Use actual current extent.
			begin_info.clearValueCount = clear_values.size();
			begin_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(_command_buffer.get(), &begin_info, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdBindPipeline(_command_buffer.get(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline._handle);
			render_pass_callback();
			vkCmdEndRenderPass(_command_buffer.get());
		});
		_command_buffer.end();
		_command_buffer.submit(_image_acquired._handle, _rendering_complete._handle);
		_swapchain.present(framebuffer_index, _rendering_complete._handle);
	}
}
