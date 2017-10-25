#ifndef _src_renderer_vulkan_buffer_h_
#define _src_renderer_vulkan_buffer_h_

#include "../buffer.h"
#include "wrappers.h"

namespace Yttrium
{
	struct VulkanIndexBuffer final : IndexBufferImpl
	{
		VK_Buffer _buffer;

		VulkanIndexBuffer(IndexFormat format, size_t count, size_t element_size, const VK_Device&);
		void write(size_t offset, size_t count, const void* data) override;
	};

	struct VulkanVertexBuffer final : VertexBufferImpl
	{
		VK_Buffer _buffer;

		VulkanVertexBuffer(size_t count, size_t element_size, const VK_Device&);
		void write(size_t offset, size_t count, const void* data) override;
	};
}

#endif
