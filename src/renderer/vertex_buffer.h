#ifndef __RENDERER_VERTEX_BUFFER_H
#define __RENDERER_VERTEX_BUFFER_H

#include <yttrium/vertex_buffer.h>

namespace Yttrium
{
	class VertexBufferImpl : public VertexBuffer
	{
	public:

		VertexBufferImpl(unsigned format, size_t size, size_t element_size)
			: _format(format)
			, _size(size)
			, _element_size(element_size)
		{
		}

		unsigned format() const override { return _format; }
		size_t size() const override { return _size; }

	protected:

		const unsigned _format;
		const size_t _size;
		const size_t _element_size;
	};
}

#endif
