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

#include <string>
#include <utility>
#include <vector>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class VulkanContext;

	template <typename T>
	class VK_Handle
	{
	public:
		VK_Handle(const VK_Handle&) = delete;
		VK_Handle& operator=(const VK_Handle&) = delete;

		auto get() const noexcept { return _handle; }
		auto release() noexcept { return std::exchange(_handle, T{ VK_NULL_HANDLE }); }

		explicit operator bool() const noexcept { return VK_NULL_HANDLE != _handle; }

	protected:
		T _handle = VK_NULL_HANDLE;

		VK_Handle() noexcept = default;
		explicit VK_Handle(T handle) noexcept
			: _handle{ handle } {}
	};

	class VK_HBuffer : public VK_Handle<VkBuffer>
	{
	public:
		explicit VK_HBuffer(VkDevice device) noexcept
			: _device{ device } {}
		~VK_HBuffer() noexcept
		{
			if (*this)
				vkDestroyBuffer(_device, _handle, nullptr);
		}

		void bind_memory(VkDeviceMemory) const;
		void create(const VkBufferCreateInfo&);
		VkMemoryRequirements memory_requirements() const noexcept;

	private:
		const VkDevice _device;
	};

	class VK_CommandBuffer : public VK_Handle<VkCommandBuffer>
	{
	public:
		explicit VK_CommandBuffer(const VulkanContext&);
		~VK_CommandBuffer() noexcept;

		void begin(VkCommandBufferUsageFlags = 0) const;
		void add_image_layout_transition(VkImage, VkImageLayout from, VkImageLayout to) const;
		void end() const;
		void submit(VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) const;
		void submit_and_wait() const;

	private:
		const VulkanContext& _context;
	};

	class VK_HDeviceMemory : public VK_Handle<VkDeviceMemory>
	{
	public:
		VK_HDeviceMemory() noexcept = default;
		explicit VK_HDeviceMemory(VkDevice device) noexcept
			: _device{ device } {}
		VK_HDeviceMemory(VK_HDeviceMemory&& memory) noexcept
			: VK_Handle{ memory._handle }, _device{ memory._device } { memory._handle = VK_NULL_HANDLE; }
		~VK_HDeviceMemory() noexcept { reset(); }
		VK_HDeviceMemory& operator=(VK_HDeviceMemory&&) noexcept;

		void allocate(const VkMemoryAllocateInfo&);
		void reset() noexcept;

	private:
		VkDevice _device = VK_NULL_HANDLE;
	};

	class VK_HImage : public VK_Handle<VkImage>
	{
	public:
		explicit VK_HImage(VkDevice device) noexcept
			: _device{ device } {}
		VK_HImage(VK_HImage&& image) noexcept
			: VK_Handle{ image._handle }, _device{ image._device } { image._handle = VK_NULL_HANDLE; }
		~VK_HImage() noexcept { reset(); }
		VK_HImage& operator=(VK_HImage&&) noexcept;

		void bind_memory(VkDeviceMemory) const;
		void create(const VkImageCreateInfo&);
		VkMemoryRequirements memory_requirements() const noexcept;
		void reset() noexcept;

	private:
		VkDevice _device = VK_NULL_HANDLE;
	};

	class VK_HImageView : public VK_Handle<VkImageView>
	{
	public:
		VK_HImageView() noexcept = default;
		explicit VK_HImageView(VkDevice device) noexcept
			: _device{ device } {}
		VK_HImageView(VK_HImageView&& view) noexcept
			: VK_Handle{ view._handle }, _device{ view._device } { view._handle = VK_NULL_HANDLE; }
		~VK_HImageView() noexcept { reset(); }
		VK_HImageView& operator=(VK_HImageView&&) noexcept;

		void create(const VkImageViewCreateInfo&);
		void reset() noexcept;

	private:
		VkDevice _device = VK_NULL_HANDLE;
	};

	struct VK_RenderPass : public VK_Handle<VkRenderPass>
	{
	public:
		explicit VK_RenderPass(VkDevice device) noexcept
			: _device{ device } {}
		VK_RenderPass(VK_RenderPass&& render_pass) noexcept
			: VK_Handle{ render_pass._handle }, _device{ render_pass._device } { render_pass._handle = VK_NULL_HANDLE; }
		~VK_RenderPass() noexcept { destroy(); }
		VK_RenderPass& operator=(VK_RenderPass&&) noexcept;

		void create(const VkRenderPassCreateInfo&);

	private:
		void destroy() noexcept;

	private:
		VkDevice _device = VK_NULL_HANDLE;
	};

	class VK_HSampler : public VK_Handle<VkSampler>
	{
	public:
		explicit VK_HSampler(VkDevice device) noexcept
			: _device{ device } {}
		VK_HSampler(VK_HSampler&& sampler) noexcept
			: VK_Handle{ sampler._handle }, _device{ sampler._device } { sampler._handle = VK_NULL_HANDLE; }
		~VK_HSampler() noexcept
		{
			if (*this)
				vkDestroySampler(_device, _handle, nullptr);
		}

		void create(const VkSamplerCreateInfo&);

	private:
		const VkDevice _device;
	};

	class VK_HSwapchain : public VK_Handle<VkSwapchainKHR>
	{
	public:
		explicit VK_HSwapchain(VkDevice device) noexcept
			: _device{ device } {}
		~VK_HSwapchain() noexcept
		{
			if (*this)
				vkDestroySwapchainKHR(_device, _handle, nullptr);
		}

		void create(const VkSwapchainCreateInfoKHR&);

	private:
		const VkDevice _device;
	};

	[[noreturn]] void vulkan_result_check(VkResult, const std::string& function);
	std::string vulkan_result_to_string(VkResult);
}

#define Y_VK_CHECK(call) \
	do \
	{ \
		if (const auto result = (call)) \
			::Yttrium::vulkan_result_check(result, #call); \
	} while (false)
