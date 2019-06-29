//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

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
		const GLsizei _index_buffer_size;
		const GLenum _index_buffer_format;

		OpenGLMesh(GlVertexArrayHandle&& vertex_array, GlBufferHandle&& vertex_buffer, GlBufferHandle&& index_buffer, GLsizei index_buffer_size, GLenum index_buffer_format)
			: _vertex_buffer{ std::move(vertex_buffer) }
			, _vertex_array{ std::move(vertex_array) }
			, _index_buffer{ std::move(index_buffer) }
			, _index_buffer_size{ index_buffer_size }
			, _index_buffer_format{ index_buffer_format }
		{
		}
	};
}
