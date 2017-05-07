#ifndef _src_renderer_gl_mesh_h_
#define _src_renderer_gl_mesh_h_

#include <yttrium/renderer/mesh.h>
#include "wrappers.h"

namespace Yttrium
{
	class OpenGLMesh final : public Mesh
	{
	public:
		const GlBufferHandle _vertex_buffer;
		const GlVertexArrayHandle _vertex_array;
		const GlBufferHandle _index_buffer;
		const size_t _index_buffer_size;
		const GLenum _index_buffer_format;

		OpenGLMesh(GlVertexArrayHandle&& vertex_array, GlBufferHandle&& vertex_buffer, GlBufferHandle&& index_buffer, size_t index_buffer_size, GLenum index_buffer_format)
			: _vertex_buffer{std::move(vertex_buffer)}
			, _vertex_array{std::move(vertex_array)}
			, _index_buffer{std::move(index_buffer)}
			, _index_buffer_size{index_buffer_size}
			, _index_buffer_format{index_buffer_format}
		{
		}
	};
}

#endif
