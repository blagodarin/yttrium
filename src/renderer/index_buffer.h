#ifndef __RENDERER_INDEX_BUFFER_H
#define __RENDERER_INDEX_BUFFER_H

#include <yttrium/index_buffer.h>

namespace Yttrium
{
	class IndexBufferImpl : public IndexBuffer
	{
	public:

		IndexBufferImpl(Format format, size_t size, size_t element_size)
			: _format(format)
			, _size(size)
			, _element_size(element_size)
		{
		}

		Format format() const override { return _format; }
		size_t size() const override { return _size; }

	protected:

		const Format _format;
		const size_t _size;
		const size_t _element_size;
	};
}

#endif
