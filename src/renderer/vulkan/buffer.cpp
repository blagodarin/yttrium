#include "buffer.h"

namespace Yttrium
{
	VulkanIndexBuffer::VulkanIndexBuffer(IndexFormat format, size_t count, size_t element_size, const VK_Device& device)
		: IndexBufferImpl{format, count, element_size}
		, _buffer{device, static_cast<uint32_t>(count * element_size)}
	{
	}

	void VulkanIndexBuffer::write(size_t offset, size_t count, const void* data)
	{
		_buffer.write(data, _element_size * count, _element_size * offset);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(size_t count, size_t element_size, const VK_Device& device)
		: VertexBufferImpl{count, element_size}
		, _buffer{device, static_cast<uint32_t>(count * element_size)}
	{
	}

	void VulkanVertexBuffer::write(size_t offset, size_t count, const void* data)
	{
		_buffer.write(data, _element_size * count, _element_size * offset);
	}
}
