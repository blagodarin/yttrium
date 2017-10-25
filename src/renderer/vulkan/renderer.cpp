#include "renderer.h"

#include "../mesh_data.h"
#include "buffer.h"
#include "gpu_program.h"
#include "mesh.h"
#include "texture.h"

#include <cassert>

namespace Yttrium
{
	VulkanRenderer::VulkanRenderer(const WindowBackend& window)
		: _context{window}
	{
	}

	std::unique_ptr<GpuProgram> VulkanRenderer::create_gpu_program(const std::string&, const std::string&)
	{
		return std::make_unique<VulkanGpuProgram>();
	}

	std::unique_ptr<IndexBuffer> VulkanRenderer::create_index_buffer(IndexFormat format, size_t count, const void* data)
	{
		const size_t element_size = (format == IndexFormat::U16) ? 2 : 4;
		auto result = std::make_unique<VulkanIndexBuffer>(format, count, element_size, _context.device());
		if (data)
			result->write(0, count * element_size, data);
		return result;
	}

	std::unique_ptr<Texture2D> VulkanRenderer::create_texture_2d(Image&& image, Flags<TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & TextureFlag::NoMipmaps);
		return std::make_unique<VulkanTexture2D>(*this, image.format(), has_mipmaps);
	}

	std::unique_ptr<VertexBuffer> VulkanRenderer::create_vertex_buffer(const std::vector<VA>& format, size_t count, const void* data)
	{
		assert(!format.empty());
		size_t element_size = 0;
		for (const auto type : format)
		{
			switch (type)
			{
			case VA::f:  element_size += sizeof(float);     break;
			case VA::f2: element_size += sizeof(float) * 2; break;
			case VA::f3: element_size += sizeof(float) * 3; break;
			case VA::f4: element_size += sizeof(float) * 4; break;
			}
		}
		auto result = std::make_unique<VulkanVertexBuffer>(count, element_size, _context.device());
		if (data)
			result->write(0, count * element_size, data);
		return result;
	}

	void VulkanRenderer::draw_mesh(const Mesh&)
	{
	}

	void VulkanRenderer::draw_triangles(const VertexBuffer&, const IndexBuffer&)
	{
	}

	void VulkanRenderer::clear()
	{
		_context.render();
	}

	std::unique_ptr<Mesh> VulkanRenderer::create_mesh(const MeshData& data)
	{
		assert(!data._vertex_format.empty());
		assert(data._vertex_data.size() > 0);
		assert(!data._indices.empty());

		size_t vertex_size = 0;
		for (const auto type : data._vertex_format)
		{
			switch (type)
			{
			case VA::f:  vertex_size += sizeof(float);     break;
			case VA::f2: vertex_size += sizeof(float) * 2; break;
			case VA::f3: vertex_size += sizeof(float) * 3; break;
			case VA::f4: vertex_size += sizeof(float) * 4; break;
			}
		}

		const auto vertex_buffer_size = data._vertex_data.size() * vertex_size;

		if (Buffer index_data; data.make_uint16_indices(index_data))
		{
			auto result = std::make_unique<VulkanMesh>(_context.device(), vertex_buffer_size, index_data.size());
			result->_vertex_buffer.write(data._vertex_data.data(), vertex_buffer_size);
			result->_index_buffer.write(index_data.data(), index_data.size());
			return result;
		}
		else
		{
			auto result = std::make_unique<VulkanMesh>(_context.device(), vertex_buffer_size, data._indices.size() * sizeof(uint32_t));
			result->_vertex_buffer.write(data._vertex_data.data(), vertex_buffer_size);
			result->_index_buffer.write(data._indices.data(), result->_index_buffer._size);
			return result;
		}
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
