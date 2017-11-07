#include "renderer.h"

#include <yttrium/math/matrix.h>
#include <yttrium/math/rect.h>
#include "../mesh_data.h"
#include "gpu_program.h"
#include "handles.h"
#include "helpers.h"
#include "mesh.h"
#include "texture.h"

namespace
{
	const std::vector<uint32_t> BuiltinVertexShader
	{
#include "2d_vs.spirv.inc"
	};

	const std::vector<uint32_t> BuiltinFragmentShader
	{
#include "2d_fs.spirv.inc"
	};
}

namespace Yttrium
{
	VulkanRenderer::VulkanRenderer(const WindowBackend& window)
		: _context{window}
		, _uniform_buffer{_context.device(), sizeof(Matrix4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT}
		, _descriptor_set_layout{_context.device(), {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}}}
		, _descriptor_pool{_context.device(), 1, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}}}
		, _descriptor_set{_descriptor_pool, _descriptor_set_layout._handle}
		, _pipeline_layout{_context.device(), {_descriptor_set_layout._handle}}
		, _vertex_shader{_context.device(), VK_SHADER_STAGE_VERTEX_BIT, ::BuiltinVertexShader}
		, _fragment_shader{_context.device(), VK_SHADER_STAGE_FRAGMENT_BIT, ::BuiltinFragmentShader}
		, _vertex_buffer{_context.device(), 1024, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT}
	{
	}

	VulkanRenderer::~VulkanRenderer() noexcept = default;

	void VulkanRenderer::clear()
	{
		_context.device().wait_idle();
		if (std::exchange(_update_descriptors, false))
			update_descriptors();
		if (!_swapchain)
			_swapchain = std::make_unique<VulkanSwapchain>(_context.device(), _context.command_pool(), _pipeline_layout, VK_ShaderModule::make_stages({&_vertex_shader, &_fragment_shader}));
		try
		{
			_swapchain->render([this](VkCommandBuffer command_buffer, const std::function<void(const std::function<void()>&)>& render_pass)
			{
				render_pass([this, command_buffer]
				{
					vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout._handle, 0, 1, &_descriptor_set._handle, 0, nullptr);

					const std::array<VkDeviceSize, 1> vertex_buffer_offsets{0};
					vkCmdBindVertexBuffers(command_buffer, 0, 1, &_vertex_buffer._handle, vertex_buffer_offsets.data());

					// TODO: Do actual rendering.
				});
			});
		}
		catch (const VK_Swapchain::OutOfDate&)
		{
			_context.device().wait_idle();
			_swapchain.reset();
		}
	}

	std::unique_ptr<GpuProgram> VulkanRenderer::create_builtin_program_2d(RendererImpl&)
	{
		return std::make_unique<VulkanGpuProgram>(*this);
	}

	std::unique_ptr<GpuProgram> VulkanRenderer::create_gpu_program(RendererImpl&, const std::string&, const std::string&)
	{
		return std::make_unique<VulkanGpuProgram>(*this);
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

	void VulkanRenderer::set_texture(const Texture2D& texture, Flags<Texture2D::Filter>)
	{
		_descriptor_texture_2d = static_cast<const VulkanTexture2D&>(texture).descriptor_image_info();
		_update_descriptors = true;
	}

	void VulkanRenderer::set_window_size(const Size&)
	{
	}

	Image VulkanRenderer::take_screenshot(const Size& window_size) const
	{
		return Image{{window_size, PixelFormat::Rgb, 24, 4, ImageOrientation::XRightYDown}};
	}

	void VulkanRenderer::update_descriptors()
	{
		const auto uniform_buffer_info = _uniform_buffer.descriptor_buffer_info();

		std::array<VkWriteDescriptorSet, 2> writes;

		writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[0].pNext = nullptr;
		writes[0].dstSet = _descriptor_set._handle;
		writes[0].dstBinding = 0;
		writes[0].dstArrayElement = 0;
		writes[0].descriptorCount = 1;
		writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writes[0].pImageInfo = nullptr;
		writes[0].pBufferInfo = &uniform_buffer_info;
		writes[0].pTexelBufferView = nullptr;

		writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes[1].pNext = nullptr;
		writes[1].dstSet = _descriptor_set._handle;
		writes[1].dstBinding = 1;
		writes[1].dstArrayElement = 0;
		writes[1].descriptorCount = 1;
		writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writes[1].pImageInfo = &*_descriptor_texture_2d;
		writes[1].pBufferInfo = nullptr;
		writes[1].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(_context.device()._handle, writes.size(), writes.data(), 0, nullptr);
	}

	const VulkanVertexFormat& VulkanRenderer::vertex_format(const std::vector<VA>& vas)
	{
		const auto i = std::find_if(_vertex_format_cache.begin(), _vertex_format_cache.end(), [&vas](const auto& format){ return format.first == vas; });
		return i != _vertex_format_cache.end() ? *i->second : *_vertex_format_cache.emplace_back(vas, std::make_unique<VulkanVertexFormat>(vas)).second;
	}
}
