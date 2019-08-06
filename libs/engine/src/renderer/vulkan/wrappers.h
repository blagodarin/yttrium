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

#include "context.h"
#include "handles.h"

#include <stdexcept>
#include <tuple>

namespace Yt
{
	class VulkanDepthBuffer;
	class VulkanVertexFormat;

	struct VK_Swapchain
	{
		struct OutOfDate : std::runtime_error
		{
			OutOfDate()
				: std::runtime_error{ "VK_ERROR_OUT_OF_DATE_KHR" } {}
		};

		const VulkanContext& _context;
		VkFormat _format = VK_FORMAT_UNDEFINED;
		VkSwapchainKHR _handle = VK_NULL_HANDLE;
		std::vector<VkImageView> _views;

		explicit VK_Swapchain(const VulkanContext&);
		~VK_Swapchain() noexcept;

		VkAttachmentDescription attachment_description() const noexcept;

		uint32_t acquire_next_image(VkSemaphore) const;
		void present(uint32_t framebuffer_index, VkSemaphore) const;
	};

	struct VK_Framebuffers
	{
		const VulkanContext& _context;
		std::vector<VkFramebuffer> _handles;

		explicit VK_Framebuffers(const VulkanContext& context) noexcept
			: _context{ context } {}
		~VK_Framebuffers() noexcept;

		void create(VkRenderPass, const VK_Swapchain&, VkImageView depth_buffer_view);
	};

	struct VK_Semaphore
	{
		const VulkanContext& _context;
		VkSemaphore _handle = VK_NULL_HANDLE;

		explicit VK_Semaphore(const VulkanContext&);
		~VK_Semaphore() noexcept { vkDestroySemaphore(_context->_device, _handle, nullptr); }
	};

	struct VK_DescriptorSetLayout
	{
		const VulkanContext& _context;
		VkDescriptorSetLayout _handle = VK_NULL_HANDLE;

		struct Binding : VkDescriptorSetLayoutBinding
		{
			Binding(VkDescriptorType type, VkShaderStageFlags flags) noexcept
			{
				binding = 0;
				descriptorType = type;
				descriptorCount = 1;
				stageFlags = flags;
				pImmutableSamplers = nullptr;
			}
		};

		VK_DescriptorSetLayout(const VulkanContext&, std::vector<Binding>&&);
		~VK_DescriptorSetLayout() noexcept { vkDestroyDescriptorSetLayout(_context->_device, _handle, nullptr); }
	};

	struct VK_DescriptorPool
	{
		const VulkanContext& _context;
		VkDescriptorPool _handle = VK_NULL_HANDLE;

		struct Size : VkDescriptorPoolSize
		{
			Size(VkDescriptorType type_, uint32_t count_) noexcept
			{
				type = type_;
				descriptorCount = count_;
			}
		};

		VK_DescriptorPool(const VulkanContext&, uint32_t max_sets, std::initializer_list<Size>, uint32_t flags = 0);
		~VK_DescriptorPool() noexcept { vkDestroyDescriptorPool(_context->_device, _handle, nullptr); }
	};

	struct VK_DescriptorSet
	{
		const VK_DescriptorPool& _pool;
		VkDescriptorSet _handle = VK_NULL_HANDLE;

		VK_DescriptorSet(const VK_DescriptorPool&, VkDescriptorSetLayout);
		~VK_DescriptorSet() noexcept { std::ignore = vkFreeDescriptorSets(_pool._context->_device, _pool._handle, 1, &_handle); }
	};

	struct VK_ShaderModule
	{
		const VulkanContext& _context;
		const VkShaderStageFlagBits _stage;
		VkShaderModule _handle = VK_NULL_HANDLE;

		VK_ShaderModule(const VulkanContext&, VkShaderStageFlagBits stage, const std::vector<uint32_t>& data);
		~VK_ShaderModule() noexcept { vkDestroyShaderModule(_context->_device, _handle, nullptr); }

		static std::vector<VkPipelineShaderStageCreateInfo> make_stages(std::initializer_list<const VK_ShaderModule*>);
	};

	struct VK_PipelineLayout
	{
		const VulkanContext& _context;
		VkPipelineLayout _handle = VK_NULL_HANDLE;

		VK_PipelineLayout(const VulkanContext&, std::initializer_list<VkDescriptorSetLayout>);
		~VK_PipelineLayout() noexcept { vkDestroyPipelineLayout(_context->_device, _handle, nullptr); }
	};

	struct VK_Pipeline
	{
		const VulkanContext& _context;
		VkPipeline _handle = VK_NULL_HANDLE;

		explicit VK_Pipeline(const VulkanContext& context) noexcept
			: _context{ context } {}
		~VK_Pipeline() noexcept;

		void create(const VK_PipelineLayout&, VkRenderPass, const VulkanVertexFormat&, const std::vector<VkPipelineShaderStageCreateInfo>&);
	};
}
