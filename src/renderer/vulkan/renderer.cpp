#include "renderer.h"

#include "../mesh_data.h"
#include "gpu_program.h"
#include "helpers.h"
#include "mesh.h"
#include "texture.h"

#include <cassert>

namespace Yttrium
{
	VulkanRenderer::VulkanRenderer(const WindowBackend& window)
		: _context{window}
	{
	}

	VulkanRenderer::~VulkanRenderer() noexcept = default;

	std::unique_ptr<GpuProgram> VulkanRenderer::create_gpu_program(const std::string&, const std::string&)
	{
		return std::make_unique<VulkanGpuProgram>();
	}

	std::unique_ptr<Texture2D> VulkanRenderer::create_texture_2d(Image&& image, Flags<TextureFlag> flags)
	{
		const auto has_mipmaps = !(flags & TextureFlag::NoMipmaps);
		return std::make_unique<VulkanTexture2D>(*this, image.format(), has_mipmaps);
	}

	void VulkanRenderer::draw_mesh(const Mesh& mesh)
	{
		static_cast<const VulkanMesh&>(mesh).draw(VK_NULL_HANDLE); // TODO: Use actual command buffer.
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

		const auto vertex_buffer_size = data._vertex_data.size() * vertex_format(data._vertex_format)._binding.stride;

		if (Buffer index_data; data.make_uint16_indices(index_data))
		{
			auto result = std::make_unique<VulkanMesh>(_context.device(), vertex_buffer_size, index_data.size(), VK_INDEX_TYPE_UINT16, data._indices.size());
			result->_vertex_buffer.write(data._vertex_data.data(), vertex_buffer_size);
			result->_index_buffer.write(index_data.data(), index_data.size());
			return result;
		}
		else
		{
			auto result = std::make_unique<VulkanMesh>(_context.device(), vertex_buffer_size, data._indices.size() * sizeof(uint32_t), VK_INDEX_TYPE_UINT32, data._indices.size());
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

	const VulkanVertexFormat& VulkanRenderer::vertex_format(const std::vector<VA>& vas)
	{
		const auto i = std::find_if(_vertex_format_cache.begin(), _vertex_format_cache.end(), [&vas](const auto& format){ return format.first == vas; });
		return i != _vertex_format_cache.end() ? *i->second : *_vertex_format_cache.emplace_back(vas, std::make_unique<VulkanVertexFormat>(vas)).second;
	}
}
