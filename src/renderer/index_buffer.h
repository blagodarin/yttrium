#ifndef __RENDERER_INDEX_BUFFER_H
#define __RENDERER_INDEX_BUFFER_H

#include <yttrium/index_buffer.h>

namespace Yttrium
{
	class IndexBufferImpl : public IndexBuffer
	{
	public:

		IndexBufferImpl(Format format, size_t size): _format(format), _size(size) {}
		~IndexBufferImpl() override = default;

		Format format() const override { return _format; }
		size_t size() const override { return _size; }

		size_t element_size() const { return _format == Format::U16 ? 2 : 4; }

	private:

		Format _format;
		size_t _size;
	};
}

#endif
