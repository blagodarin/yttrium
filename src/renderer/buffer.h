#ifndef _src_renderer_buffer_h_
#define _src_renderer_buffer_h_

#include <yttrium/renderer/index_buffer.h>
#include <yttrium/renderer/vertex_buffer.h>

#include <yttrium/std/vector.h>

#include <initializer_list>

namespace Yttrium
{
	class IndexBufferImpl : public IndexBuffer
	{
	public:
		IndexBufferImpl(IndexFormat format, size_t size, size_t element_size)
			: _size(size)
			, _element_size(element_size)
			, _format(format)
		{
		}

		IndexFormat format() const override { return _format; }
		size_t size() const override { return _size; }

	protected:
		const size_t _size;
		const size_t _element_size;
		const IndexFormat _format;
	};

	class VertexBufferImpl : public VertexBuffer
	{
	public:
		VertexBufferImpl(size_t count, size_t element_size)
			: _count(count)
			, _element_size(element_size)
		{
		}

		size_t count() const { return _count; }

	protected:
		const size_t _count;
		const size_t _element_size;
	};
}

#endif
