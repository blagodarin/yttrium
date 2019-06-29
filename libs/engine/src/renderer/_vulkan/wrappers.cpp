//
// Copyright 2019 Sergei Blagodarin
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

#include "wrappers.h"

#include "helpers.h"

#include <cassert>
#include <cstring>

#ifndef NDEBUG
#	include <iostream>
#endif

#define CHECK(call) Y_VK_CHECK(call)

namespace Yttrium
{
	VK_Swapchain::VK_Swapchain(const VulkanContext& context)
		: _context{ context }
	{
		const auto surface_formats = _context.surface_formats();
		_format = surface_formats.empty() ? VK_FORMAT_B8G8R8A8_UNORM : surface_formats[0].format;

		VkSwapchainCreateInfoKHR swapchain_info;
		swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_info.pNext = nullptr;
		swapchain_info.flags = 0;
		swapchain_info.surface = _context->_surface;
		swapchain_info.minImageCount = _context->_surface_capabilities.minImageCount;
		swapchain_info.imageFormat = _format;
		swapchain_info.imageColorSpace = surface_formats.empty() ? VK_COLOR_SPACE_SRGB_NONLINEAR_KHR : surface_formats[0].colorSpace;
		swapchain_info.imageExtent = _context->_surface_capabilities.currentExtent;
		swapchain_info.imageArrayLayers = 1;
		swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_info.queueFamilyIndexCount = 0;
		swapchain_info.pQueueFamilyIndices = nullptr;
		swapchain_info.preTransform = _context.surface_transform();
		swapchain_info.compositeAlpha = _context.composite_alpha();
		swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchain_info.clipped = VK_TRUE;
		swapchain_info.oldSwapchain = VK_NULL_HANDLE;

		VK_HSwapchain swapchain{ _context->_device };
		swapchain.create(swapchain_info);

		uint32_t image_count = 0;
		CHECK(vkGetSwapchainImagesKHR(_context->_device, swapchain.get(), &image_count, nullptr));

		std::vector<VkImage> images(image_count, VK_NULL_HANDLE);
		CHECK(vkGetSwapchainImagesKHR(_context->_device, swapchain.get(), &image_count, images.data()));

		std::vector<VK_HImageView> views;
		views.reserve(image_count);
		for (const auto image : images)
		{
			VkImageViewCreateInfo view_info;
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.pNext = nullptr;
			view_info.flags = 0;
			view_info.image = image;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = _format;
			view_info.components.r = VK_COMPONENT_SWIZZLE_R;
			view_info.components.g = VK_COMPONENT_SWIZZLE_G;
			view_info.components.b = VK_COMPONENT_SWIZZLE_B;
			view_info.components.a = VK_COMPONENT_SWIZZLE_A;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;
			views.emplace_back(_context->_device).create(view_info);
		}

		_views.reserve(views.size());

		_handle = swapchain.release();
		for (auto& view : views)
			_views.emplace_back(view.release());
	}

	VK_Swapchain::~VK_Swapchain() noexcept
	{
		for (const auto view : _views)
			vkDestroyImageView(_context->_device, view, nullptr);
		vkDestroySwapchainKHR(_context->_device, _handle, nullptr);
	}

	VkAttachmentDescription VK_Swapchain::attachment_description() const noexcept
	{
		VkAttachmentDescription description;
		description.flags = 0;
		description.format = _format;
		description.samples = VK_SAMPLE_COUNT_1_BIT;
		description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		return description;
	}

	uint32_t VK_Swapchain::acquire_next_image(VkSemaphore semaphore) const
	{
		uint32_t index = 0;
		switch (const auto result = vkAcquireNextImageKHR(_context->_device, _handle, std::numeric_limits<uint64_t>::max(), semaphore, VK_NULL_HANDLE, &index))
		{
		case VK_SUCCESS: return index;
		case VK_ERROR_OUT_OF_DATE_KHR: throw OutOfDate{};
		default: throw std::runtime_error{ "vkAcquireNextImageKHR = " + vulkan_result_to_string(result) };
		}
	}

	void VK_Swapchain::present(uint32_t framebuffer_index, VkSemaphore semaphore) const
	{
		VkPresentInfoKHR present_info;
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.pNext = nullptr;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &semaphore;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &_handle;
		present_info.pImageIndices = &framebuffer_index;
		present_info.pResults = nullptr;

		switch (const auto result = vkQueuePresentKHR(_context->_queue, &present_info))
		{
		case VK_SUCCESS: return;
		case VK_ERROR_OUT_OF_DATE_KHR: throw OutOfDate{};
		default: throw std::runtime_error{ "vkQueuePresentKHR = " + vulkan_result_to_string(result) };
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Framebuffers::~VK_Framebuffers() noexcept
	{
		for (const auto handle : _handles)
			vkDestroyFramebuffer(_context->_device, handle, nullptr);
	}

	void VK_Framebuffers::create(VkRenderPass render_pass, const VK_Swapchain& swapchain, VkImageView depth_buffer_view)
	{
		std::array<VkImageView, 2> attachments{ VK_NULL_HANDLE, depth_buffer_view };

		VkFramebufferCreateInfo create_info;
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.renderPass = render_pass;
		create_info.attachmentCount = attachments.size();
		create_info.pAttachments = attachments.data();
		create_info.width = _context->_surface_capabilities.currentExtent.width;
		create_info.height = _context->_surface_capabilities.currentExtent.height;
		create_info.layers = 1;

		_handles.reserve(swapchain._views.size());
		for (const auto swapchain_view : swapchain._views)
		{
			attachments[0] = swapchain_view;

			VkFramebuffer framebuffer = VK_NULL_HANDLE;
			CHECK(vkCreateFramebuffer(_context->_device, &create_info, nullptr, &framebuffer));
			_handles.emplace_back(framebuffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Semaphore::VK_Semaphore(const VulkanContext& context)
		: _context{ context }
	{
		VkSemaphoreCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		CHECK(vkCreateSemaphore(_context->_device, &info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DescriptorSetLayout::VK_DescriptorSetLayout(const VulkanContext& context, std::vector<Binding>&& bindings)
		: _context{ context }
	{
		for (auto& binding : bindings)
			binding.binding = static_cast<uint32_t>(&binding - bindings.data());

		VkDescriptorSetLayoutCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.bindingCount = static_cast<uint32_t>(bindings.size());
		info.pBindings = bindings.data();
		CHECK(vkCreateDescriptorSetLayout(_context->_device, &info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DescriptorPool::VK_DescriptorPool(const VulkanContext& context, uint32_t max_sets, std::initializer_list<Size> sizes, uint32_t flags)
		: _context{ context }
	{
		VkDescriptorPoolCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = flags;
		info.maxSets = max_sets;
		info.poolSizeCount = static_cast<uint32_t>(sizes.size());
		info.pPoolSizes = sizes.begin();

		CHECK(vkCreateDescriptorPool(_context->_device, &info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_DescriptorSet::VK_DescriptorSet(const VK_DescriptorPool& pool, VkDescriptorSetLayout layout)
		: _pool{ pool }
	{
		VkDescriptorSetAllocateInfo info;
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.descriptorPool = _pool._handle;
		info.descriptorSetCount = 1;
		info.pSetLayouts = &layout;

		CHECK(vkAllocateDescriptorSets(_pool._context->_device, &info, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_ShaderModule::VK_ShaderModule(const VulkanContext& context, VkShaderStageFlagBits stage, const std::vector<uint32_t>& data)
		: _context{ context }
		, _stage{ stage }
	{
		VkShaderModuleCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.codeSize = data.size() * sizeof(uint32_t);
		info.pCode = data.data();

		CHECK(vkCreateShaderModule(_context->_device, &info, nullptr, &_handle));
	}

	std::vector<VkPipelineShaderStageCreateInfo> VK_ShaderModule::make_stages(std::initializer_list<const VK_ShaderModule*> modules)
	{
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		stages.reserve(modules.size());
		for (const auto module : modules)
		{
			auto& stage = stages.emplace_back();
			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.pNext = nullptr;
			stage.flags = 0;
			stage.stage = module->_stage;
			stage.module = module->_handle;
			stage.pName = "main";
			stage.pSpecializationInfo = nullptr;
		}
		return stages;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_PipelineLayout::VK_PipelineLayout(const VulkanContext& context, std::initializer_list<VkDescriptorSetLayout> set_layouts)
		: _context{ context }
	{
		VkPipelineLayoutCreateInfo info;
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
		info.pSetLayouts = set_layouts.begin();
		info.pushConstantRangeCount = 0;
		info.pPushConstantRanges = nullptr;

		CHECK(vkCreatePipelineLayout(_context->_device, &info, nullptr, &_handle));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VK_Pipeline::~VK_Pipeline() noexcept
	{
		if (_handle != VK_NULL_HANDLE)
			vkDestroyPipeline(_context->_device, _handle, nullptr);
	}

	void VK_Pipeline::create(const VK_PipelineLayout& layout, VkRenderPass render_pass, const VulkanVertexFormat& vertex_format, const std::vector<VkPipelineShaderStageCreateInfo>& shader_stages)
	{
		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.pNext = nullptr;
		viewport_state.flags = 0;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = nullptr;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = nullptr;

		VkPipelineRasterizationStateCreateInfo rasterization_state = {};
		rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterization_state.pNext = nullptr;
		rasterization_state.flags = 0;
		rasterization_state.depthClampEnable = VK_TRUE;
		rasterization_state.rasterizerDiscardEnable = VK_FALSE;
		rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
		rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterization_state.depthBiasEnable = VK_FALSE;
		rasterization_state.depthBiasConstantFactor = 0.f;
		rasterization_state.depthBiasClamp = 0.f;
		rasterization_state.depthBiasSlopeFactor = 0.f;
		rasterization_state.lineWidth = 1.f;

		VkPipelineMultisampleStateCreateInfo multisample_state = {};
		multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisample_state.pNext = nullptr;
		multisample_state.flags = 0;
		multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisample_state.sampleShadingEnable = VK_FALSE;
		multisample_state.minSampleShading = 0.f;
		multisample_state.pSampleMask = nullptr;
		multisample_state.alphaToCoverageEnable = VK_FALSE;
		multisample_state.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {};
		depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil_state.pNext = nullptr;
		depth_stencil_state.flags = 0;
		depth_stencil_state.depthTestEnable = VK_TRUE;
		depth_stencil_state.depthWriteEnable = VK_TRUE;
		depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
		depth_stencil_state.stencilTestEnable = VK_FALSE;
		depth_stencil_state.front.failOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.passOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.depthFailOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.front.compareOp = VK_COMPARE_OP_ALWAYS;
		depth_stencil_state.front.compareMask = 0;
		depth_stencil_state.front.writeMask = 0;
		depth_stencil_state.front.reference = 0;
		depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.back.depthFailOp = VK_STENCIL_OP_KEEP;
		depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depth_stencil_state.back.compareMask = 0;
		depth_stencil_state.back.writeMask = 0;
		depth_stencil_state.back.reference = 0;
		depth_stencil_state.minDepthBounds = 0;
		depth_stencil_state.maxDepthBounds = 0;

		VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
		color_blend_attachment_state.blendEnable = VK_FALSE;
		color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
		color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo color_blend_state = {};
		color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blend_state.pNext = nullptr;
		color_blend_state.flags = 0;
		color_blend_state.logicOpEnable = VK_FALSE;
		color_blend_state.logicOp = VK_LOGIC_OP_NO_OP;
		color_blend_state.attachmentCount = 1;
		color_blend_state.pAttachments = &color_blend_attachment_state;
		color_blend_state.blendConstants[0] = 1.f;
		color_blend_state.blendConstants[1] = 1.f;
		color_blend_state.blendConstants[2] = 1.f;
		color_blend_state.blendConstants[3] = 1.f;

		const std::array<VkDynamicState, 2> dynamic_state_data{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

		VkPipelineDynamicStateCreateInfo dynamic_state = {};
		dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state.pNext = nullptr;
		dynamic_state.flags = 0;
		dynamic_state.dynamicStateCount = dynamic_state_data.size();
		dynamic_state.pDynamicStates = dynamic_state_data.data();

		VkGraphicsPipelineCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		create_info.pNext = nullptr;
		create_info.flags = 0;
		create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
		create_info.pStages = shader_stages.data();
		create_info.pVertexInputState = &vertex_format._input;
		create_info.pInputAssemblyState = &vertex_format._assembly;
		create_info.pTessellationState = nullptr;
		create_info.pViewportState = &viewport_state;
		create_info.pRasterizationState = &rasterization_state;
		create_info.pMultisampleState = &multisample_state;
		create_info.pDepthStencilState = &depth_stencil_state;
		create_info.pColorBlendState = &color_blend_state;
		create_info.pDynamicState = &dynamic_state;
		create_info.layout = layout._handle;
		create_info.renderPass = render_pass;
		create_info.subpass = 0;
		create_info.basePipelineHandle = VK_NULL_HANDLE;
		create_info.basePipelineIndex = 0;

		CHECK(vkCreateGraphicsPipelines(_context->_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &_handle));
	}
}
