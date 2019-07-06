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

#include "handles.h"

#include "context.h"

#include <cassert>
#include <stdexcept>

namespace Yttrium
{
	void VK_HBuffer::bind_memory(VkDeviceMemory memory) const
	{
		assert(*this && VK_NULL_HANDLE != memory);
		Y_VK_CHECK(vkBindBufferMemory(_device, _handle, memory, 0));
	}

	void VK_HBuffer::create(const VkBufferCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateBuffer(_device, &info, nullptr, &_handle));
	}

	VkMemoryRequirements VK_HBuffer::memory_requirements() const noexcept
	{
		assert(*this);
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(_device, _handle, &requirements);
		return requirements;
	}

	VK_CommandBuffer::VK_CommandBuffer(const VulkanContext& context)
		: _context{ context }
	{
		VkCommandBufferAllocateInfo info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.commandPool = _context->_command_pool;
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		info.commandBufferCount = 1;
		Y_VK_CHECK(vkAllocateCommandBuffers(_context->_device, &info, &_handle));
	}

	VK_CommandBuffer::~VK_CommandBuffer() noexcept
	{
		vkFreeCommandBuffers(_context->_device, _context->_command_pool, 1, &_handle);
	}

	void VK_CommandBuffer::begin(VkCommandBufferUsageFlags flags) const
	{
		VkCommandBufferBeginInfo info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;
		info.flags = flags;
		info.pInheritanceInfo = nullptr;
		Y_VK_CHECK(vkBeginCommandBuffer(_handle, &info));
	}

	void VK_CommandBuffer::add_image_layout_transition(VkImage image, VkImageLayout from, VkImageLayout to) const
	{
		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;
		barrier.oldLayout = from;
		barrier.newLayout = to;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags src_stage = 0;
		VkPipelineStageFlags dst_stage = 0;
		if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (from == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
			throw std::logic_error{ "Bad image layout transition" };

		vkCmdPipelineBarrier(_handle, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

	void VK_CommandBuffer::end() const
	{
		Y_VK_CHECK(vkEndCommandBuffer(_handle));
	}

	void VK_CommandBuffer::submit(VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) const
	{
		const VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &wait_semaphore;
		submit_info.pWaitDstStageMask = &pipeline_stage_flags;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_handle;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &signal_semaphore;

		Y_VK_CHECK(vkQueueSubmit(_context->_queue, 1, &submit_info, VK_NULL_HANDLE));
	}

	void VK_CommandBuffer::submit_and_wait() const
	{
		VkSubmitInfo info;
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;
		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &_handle;
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;
		Y_VK_CHECK(vkQueueSubmit(_context->_queue, 1, &info, VK_NULL_HANDLE));
		Y_VK_CHECK(vkQueueWaitIdle(_context->_queue));
	}

	VK_HDeviceMemory& VK_HDeviceMemory::operator=(VK_HDeviceMemory&& memory) noexcept
	{
		reset();
		_handle = memory._handle;
		_device = memory._device;
		memory._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_HDeviceMemory::allocate(const VkMemoryAllocateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkAllocateMemory(_device, &info, nullptr, &_handle));
	}

	void VK_HDeviceMemory::reset() noexcept
	{
		if (*this)
			vkFreeMemory(_device, _handle, nullptr);
	}

	VK_HImage& VK_HImage::operator=(VK_HImage&& image) noexcept
	{
		reset();
		_handle = image._handle;
		_device = image._device;
		image._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_HImage::bind_memory(VkDeviceMemory memory) const
	{
		assert(*this && VK_NULL_HANDLE != memory);
		Y_VK_CHECK(vkBindImageMemory(_device, _handle, memory, 0));
	}

	void VK_HImage::create(const VkImageCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateImage(_device, &info, nullptr, &_handle));
	}

	VkMemoryRequirements VK_HImage::memory_requirements() const noexcept
	{
		assert(*this);
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(_device, _handle, &requirements);
		return requirements;
	}

	void VK_HImage::reset() noexcept
	{
		if (*this)
			vkDestroyImage(_device, _handle, nullptr);
	}

	VK_HImageView& VK_HImageView::operator=(VK_HImageView&& view) noexcept
	{
		reset();
		_handle = view._handle;
		_device = view._device;
		view._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_HImageView::create(const VkImageViewCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateImageView(_device, &info, nullptr, &_handle));
	}

	void VK_HImageView::reset() noexcept
	{
		if (*this)
			vkDestroyImageView(_device, _handle, nullptr);
	}

	VK_RenderPass& VK_RenderPass::operator=(VK_RenderPass&& render_pass) noexcept
	{
		destroy();
		_handle = render_pass._handle;
		_device = render_pass._device;
		render_pass._handle = VK_NULL_HANDLE;
		return *this;
	}

	void VK_RenderPass::create(const VkRenderPassCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateRenderPass(_device, &info, nullptr, &_handle));
	}

	void VK_RenderPass::destroy() noexcept
	{
		if (*this)
			vkDestroyRenderPass(_device, _handle, nullptr);
	}

	void VK_HSampler::create(const VkSamplerCreateInfo& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateSampler(_device, &info, nullptr, &_handle));
	}

	void VK_HSwapchain::create(const VkSwapchainCreateInfoKHR& info)
	{
		assert(!*this);
		Y_VK_CHECK(vkCreateSwapchainKHR(_device, &info, nullptr, &_handle));
	}

	void vulkan_result_check(VkResult result, const std::string& function)
	{
		throw std::runtime_error{ function.substr(0, function.find('(')) + " = " + vulkan_result_to_string(result) };
	}

	std::string vulkan_result_to_string(VkResult result)
	{
		switch (result)
		{
		case VK_SUCCESS: return "VK_SUCCESS";
		case VK_NOT_READY: return "VK_NOT_READY";
		case VK_TIMEOUT: return "VK_TIMEOUT";
		case VK_EVENT_SET: return "VK_EVENT_SET";
		case VK_EVENT_RESET: return "VK_EVENT_RESET";
		case VK_INCOMPLETE: return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
#if VK_HEADER_VERSION < 54
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHX: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHX";
#else
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
#endif
		default: return std::to_string(result);
		}
	}
}
