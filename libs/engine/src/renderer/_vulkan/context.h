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

#include <vector>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	class VK_CommandBuffer;
	class VK_HDeviceMemory;
	class VK_HImage;
	class VK_HImageView;
	class VK_HSampler;
	class WindowBackend;

	class VulkanContext
	{
	public:
		explicit VulkanContext(const WindowBackend&);
		~VulkanContext() noexcept = default;

		const auto* operator-> () const noexcept { return &_data; }

		VK_CommandBuffer allocate_command_buffer() const;
		VK_HDeviceMemory allocate_memory(const VkMemoryRequirements&, VkMemoryPropertyFlags) const;
		VK_HImage create_texture_2d_image(size_t width, size_t height, VkFormat);
		VK_HSampler create_texture_2d_sampler();
		VK_HImageView create_texture_2d_view(VkImage, VkFormat);

		VkCompositeAlphaFlagBitsKHR composite_alpha() const noexcept;
		uint32_t memory_type_index(uint32_t type_bits, VkFlags) const;
		std::vector<VkSurfaceFormatKHR> surface_formats() const;
		VkSurfaceTransformFlagBitsKHR surface_transform() const noexcept;
		VkImageTiling tiling(VkFormat, VkFlags) const;

		void wait_idle() const;

	private:
		struct Data
		{
			VkInstance _instance = VK_NULL_HANDLE;
#ifndef NDEBUG
			PFN_vkDestroyDebugReportCallbackEXT _destroy_debug_report_callback = nullptr;
			VkDebugReportCallbackEXT _debug_report_callback = VK_NULL_HANDLE;
#endif
			VkSurfaceKHR _surface = VK_NULL_HANDLE;
			VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
			uint32_t _queue_family_index = 0;
			VkPhysicalDeviceFeatures _physical_device_features;
			VkPhysicalDeviceMemoryProperties _physical_device_memory_properties;
			VkSurfaceCapabilitiesKHR _surface_capabilities;
			VkDevice _device = VK_NULL_HANDLE;
			VkQueue _queue = VK_NULL_HANDLE;
			VkCommandPool _command_pool = VK_NULL_HANDLE;

			Data() noexcept = default;
			~Data() noexcept;

			void create(const WindowBackend&);
		};

		Data _data;
	};
}
