#include "renderer.h"

#include <yttrium/math/rect.h>
#include "../mesh_data.h"
#include "gpu_program.h"
#include "handles.h"
#include "helpers.h"
#include "mesh.h"
#include "texture.h"

namespace Yttrium
{
	VulkanRenderer::VulkanRenderer(const WindowBackend& window)
		: _context{window}
	{
	}

	VulkanRenderer::~VulkanRenderer() noexcept = default;

	void VulkanRenderer::clear()
	{
		_context.render();
	}

	std::unique_ptr<GpuProgram> VulkanRenderer::create_builtin_program_2d(RendererImpl&)
	{
		return std::make_unique<VulkanGpuProgram>();
	}

	std::unique_ptr<GpuProgram> VulkanRenderer::create_gpu_program(RendererImpl&, const std::string&, const std::string&)
	{
		return std::make_unique<VulkanGpuProgram>();
	}

	std::unique_ptr<Mesh> VulkanRenderer::create_mesh(const MeshData& data)
	{
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
			const auto index_buffer_size = data._indices.size() * sizeof(uint32_t);
			auto result = std::make_unique<VulkanMesh>(_context.device(), vertex_buffer_size, index_buffer_size, VK_INDEX_TYPE_UINT32, data._indices.size());
			result->_vertex_buffer.write(data._vertex_data.data(), vertex_buffer_size);
			result->_index_buffer.write(data._indices.data(), index_buffer_size);
			return result;
		}
	}

	std::unique_ptr<Texture2D> VulkanRenderer::create_texture_2d(RendererImpl& renderer, Image&& image, Flags<Renderer::TextureFlag> flags)
	{
		if (flags & Renderer::TextureFlag::Intensity)
		{
			auto converted = intensity_to_bgra(image);
			if (converted)
				image = std::move(*converted);
		}

		const auto format = image.format();
		if (format.bits_per_channel() != 8)
			return {};

		auto data = image.data();

		VkFormat vk_format = VK_FORMAT_UNDEFINED;
		std::optional<Image> temporary;
		switch (format.pixel_format())
		{
		case PixelFormat::Gray:
		case PixelFormat::GrayAlpha:
			temporary = grayscale_to_bgra(image);
			data = temporary->data();
			vk_format = VK_FORMAT_B8G8R8A8_UNORM;
			break;
		case PixelFormat::Rgb:
			vk_format = VK_FORMAT_R8G8B8_UNORM;
			break;
		case PixelFormat::Bgr:
			vk_format = VK_FORMAT_B8G8R8_UNORM;
			break;
		case PixelFormat::Rgba:
			vk_format = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case PixelFormat::Bgra:
			vk_format = VK_FORMAT_B8G8R8A8_UNORM;
			break;
		default:
			return {};
		}

		const auto has_mipmaps = !(flags & Renderer::TextureFlag::NoMipmaps);
		return std::make_unique<VulkanTexture2D>(renderer, _context, image.format(), has_mipmaps, vk_format, data);
	}

	size_t VulkanRenderer::draw_mesh(const Mesh& mesh)
	{
		static_cast<const VulkanMesh&>(mesh).draw(VK_NULL_HANDLE); // TODO: Use actual command buffer.
		return 0;
	}

	void VulkanRenderer::flush_2d(const Buffer&, const Buffer&)
	{
	}

	RectF VulkanRenderer::map_rect(const RectF& rect, ImageOrientation) const
	{
		return rect;
	}

	void VulkanRenderer::set_program(const GpuProgram*)
	{
	}

	void VulkanRenderer::set_texture(const Texture2D&, Flags<Texture2D::Filter>)
	{
	}

	void VulkanRenderer::set_window_size(const Size&)
	{
	}

	Image VulkanRenderer::take_screenshot(const Size& window_size) const
	{
		return Image{{window_size, PixelFormat::Rgb, 24, 4, ImageOrientation::XRightYDown}};
	}

	const VulkanVertexFormat& VulkanRenderer::vertex_format(const std::vector<VA>& vas)
	{
		const auto i = std::find_if(_vertex_format_cache.begin(), _vertex_format_cache.end(), [&vas](const auto& format){ return format.first == vas; });
		return i != _vertex_format_cache.end() ? *i->second : *_vertex_format_cache.emplace_back(vas, std::make_unique<VulkanVertexFormat>(vas)).second;
	}
}
