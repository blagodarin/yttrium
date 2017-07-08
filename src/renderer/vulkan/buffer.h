#ifndef _src_renderer_vulkan_buffer_h_
#define _src_renderer_vulkan_buffer_h_

#include "../buffer.h"

namespace Yttrium
{
	class VulkanIndexBuffer final : public IndexBufferImpl
	{
	public:
		VulkanIndexBuffer(IndexFormat format, size_t size, size_t element_size) : IndexBufferImpl{format, size, element_size} {}
		void write(size_t, size_t, const void*) override {}
	};

	class VulkanVertexBuffer final : public VertexBufferImpl
	{
	public:
		VulkanVertexBuffer(size_t count, size_t element_size) : VertexBufferImpl{count, element_size} {}
		void write(size_t, size_t, const void*) override {}
	};
}

#endif
