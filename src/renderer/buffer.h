#ifndef _src_renderer_buffer_h_
#define _src_renderer_buffer_h_

#include <yttrium/index_buffer.h>
#include <yttrium/vertex_buffer.h>

#include <yttrium/std/vector.h>

#include <initializer_list>

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

		VertexBufferImpl(std::initializer_list<VA> format, size_t size, size_t element_size, Allocator& allocator)
			: _size(size)
			, _element_size(element_size)
			, _format(format, allocator)
		{
		}

		size_t size() const override { return _size; }

		size_t element_size() const { return _element_size; }

		const StdVector<VA>& format() const { return _format; }

		static size_t element_size(std::initializer_list<VA> format)
		{
			size_t result = 0;
			for (const auto type : format)
			{
				switch (type)
				{
				case VA::f: result += sizeof(float); break;
				case VA::f2: result += sizeof(float) * 2; break;
				case VA::f3: result += sizeof(float) * 3; break;
				case VA::f4: result += sizeof(float) * 4; break;
				}
			}
			return result;
		}

	protected:

		const size_t _size;
		const size_t _element_size;
		const StdVector<VA> _format;
	};
}

#endif
