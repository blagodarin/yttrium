#ifndef __RENDERER_BUFFER_H
#define __RENDERER_BUFFER_H

#include <yttrium/index_buffer.h>
#include <yttrium/vertex_buffer.h>

namespace Yttrium
{
	class IndexBufferImpl : public IndexBuffer
	{
	public:

		IndexBufferImpl(Format format, size_t size, size_t element_size)
			: _size(size)
			, _element_size(element_size)
			, _format(format)
		{
		}

		Format format() const override { return _format; }
		size_t size() const override { return _size; }

	protected:

		const size_t _size;
		const size_t _element_size;
		const Format _format;
	};

	class VertexBufferImpl : public VertexBuffer
	{
	public:

		VertexBufferImpl(unsigned format, size_t size, size_t element_size)
			: _size(size)
			, _element_size(element_size)
			, _format(format)
		{
		}

		unsigned format() const override { return _format; }
		size_t size() const override { return _size; }

		size_t element_size() const { return _element_size; }

	protected:

		const size_t _size;
		const size_t _element_size;
		const unsigned _format;
	};
}

#endif
