#include "renderer.h"

#include "../mesh_data.h"
#include "buffer.h"
#include "gpu_program.h"
#include "mesh.h"
#include "texture.h"

#include <cassert>

namespace Yttrium
{
	std::unique_ptr<GpuProgram> VulkanRenderer::create_gpu_program(const std::string&, const std::string&)
	{
		return std::make_unique<VulkanGpuProgram>();
	}

	std::unique_ptr<IndexBuffer> VulkanRenderer::create_index_buffer(IndexFormat format, size_t count, const void*)
	{
		const size_t element_size = (format == IndexFormat::U16) ? 2 : 4;
		return std::make_unique<VulkanIndexBuffer>(format, count, element_size);
	}

	std::unique_ptr<Texture2D> VulkanRenderer::create_texture_2d(Image&& image, Flags<TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & TextureFlag::NoMipmaps);
		return std::make_unique<VulkanTexture2D>(*this, image.format(), has_mipmaps);
	}

	std::unique_ptr<VertexBuffer> VulkanRenderer::create_vertex_buffer(const std::vector<VA>& format, size_t count, const void*)
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

		return std::make_unique<VulkanVertexBuffer>(count, offset);
	}

	void VulkanRenderer::draw_mesh(const Mesh&)
	{
	}

	void VulkanRenderer::draw_triangles(const VertexBuffer&, const IndexBuffer&)
	{
	}

	void VulkanRenderer::clear()
	{
	}

	std::unique_ptr<Mesh> VulkanRenderer::create_mesh(const MeshData& data)
	{
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());

		return std::make_unique<VulkanMesh>();
	}

	RectF VulkanRenderer::map_rect(const RectF& rect, ImageOrientation) const
	{
		return rect;
	}

	Image VulkanRenderer::take_screenshot() const
	{
		return Image{{window_size(), PixelFormat::Rgb, 24, 4, ImageOrientation::XRightYDown}};
	}

	void VulkanRenderer::flush_2d_impl(const Buffer&, const Buffer&)
	{
	}

	void VulkanRenderer::set_program(const GpuProgram*)
	{
	}

	void VulkanRenderer::set_texture(const Texture2D&, Flags<Texture2D::Filter>)
	{
	}

	void VulkanRenderer::set_window_size_impl(const Size&)
	{
	}
}
