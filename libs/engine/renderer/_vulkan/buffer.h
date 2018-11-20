//
// Copyright 2018 Sergei Blagodarin
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

#ifndef _src_renderer_vulkan_buffer_h_
#define _src_renderer_vulkan_buffer_h_

#include <vulkan/vulkan.h>

namespace Yttrium
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

#endif
