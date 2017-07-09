#include "renderer.h"

#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/mesh.h>
#include "../buffer.h"
#include "../mesh_data.h"
#include "../texture.h"

#include <cassert>

namespace Yttrium
{
	class NullGpuProgram final : public GpuProgram
	{
	public:
		void set_uniform(const std::string&, const Matrix4&) override {}
	};

	class NullIndexBuffer final : public IndexBufferImpl
	{
	public:
		NullIndexBuffer(IndexFormat format, size_t size, size_t element_size) : IndexBufferImpl{format, size, element_size} {}
		void write(size_t, size_t, const void*) override {}
	};

	class NullVertexBuffer final : public VertexBufferImpl
	{
	public:
		NullVertexBuffer(size_t count, size_t element_size) : VertexBufferImpl{count, element_size} {}
		void write(size_t, size_t, const void*) override {}
	};

	std::unique_ptr<GpuProgram> NullRenderer::create_gpu_program(const std::string&, const std::string&)
	{
		return std::make_unique<NullGpuProgram>();
	}

	std::unique_ptr<IndexBuffer> NullRenderer::create_index_buffer(IndexFormat format, size_t count, const void*)
	{
		const size_t element_size = (format == IndexFormat::U16) ? 2 : 4;
		return std::make_unique<NullIndexBuffer>(format, count, element_size);
	}

	std::unique_ptr<Texture2D> NullRenderer::create_texture_2d(Image&& image, Flags<TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & TextureFlag::NoMipmaps);
		return std::make_unique<BackendTexture2D>(*this, image.format(), has_mipmaps);
	}

	std::unique_ptr<VertexBuffer> NullRenderer::create_vertex_buffer(const std::vector<VA>& format, size_t count, const void*)
	{
		assert(!format.empty());

		size_t offset = 0;
		for (const auto type : format)
		{
			switch (type)
			{
			case VA::f:
				offset += sizeof(float);
				break;
			case VA::f2:
				offset += sizeof(float) * 2;
				break;
			case VA::f3:
				offset += sizeof(float) * 3;
				break;
			case VA::f4:
				offset += sizeof(float) * 4;
				break;
			}
		}

		return std::make_unique<NullVertexBuffer>(count, offset);
	}

	std::unique_ptr<Mesh> NullRenderer::create_mesh(const MeshData& data)
	{
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());

		return std::make_unique<Mesh>();
	}

	Image NullRenderer::take_screenshot() const
	{
		return Image{{window_size(), PixelFormat::Rgb, 24, 4, ImageOrientation::XRightYDown}};
	}
}
