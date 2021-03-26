// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include "renderer.h"

#include <yttrium/base/numeric.h>
#include <yttrium/geometry/matrix.h>
#include <yttrium/geometry/rect.h>
#include "../../model/mesh_data.h"
#include "handles.h"
#include "helpers.h"
#include "mesh.h"
#include "program.h"
#include "swapchain.h"
#include "texture.h"

#include <array>

// TODO: Finish Vulkan implementation.

namespace
{
	const std::vector<uint32_t> BuiltinVertexShader{
#include "2d_vs.glsl.spirv.inc"
	};

	const std::vector<uint32_t> BuiltinFragmentShader{
#include "2d_fs.glsl.spirv.inc"
	};
}

namespace Yt
{
	VulkanRenderer::VulkanRenderer(const WindowID& window_id)
		: _context{ window_id }
		, _uniform_buffer{ _context, sizeof(Matrix4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
		, _descriptor_set_layout{ _context, { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT } } }
		, _descriptor_pool{ _context, 1, { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 } } }
		, _descriptor_set{ _descriptor_pool, _descriptor_set_layout._handle }
		, _pipeline_layout{ _context, { _descriptor_set_layout._handle } }
		, _vertex_shader{ _context, VK_SHADER_STAGE_VERTEX_BIT, ::BuiltinVertexShader }
		, _fragment_shader{ _context, VK_SHADER_STAGE_FRAGMENT_BIT, ::BuiltinFragmentShader }
		, _vertex_buffer{ _context, 1024, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
	{
	}

	VulkanRenderer::~VulkanRenderer() noexcept = default;

	void VulkanRenderer::clear()
	{
		_context.wait_idle();
		if (std::exchange(_update_descriptors, false))
			update_descriptors();
		if (!_swapchain)
			_swapchain = std::make_unique<VulkanSwapchain>(_context, _pipeline_layout, VK_ShaderModule::make_stages({ &_vertex_shader, &_fragment_shader }));
		try
		{
			_swapchain->render([this](VkCommandBuffer command_buffer, const std::function<void(const std::function<void()>&)>& render_pass) {
				render_pass([this, command_buffer] {
					vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout._handle, 0, 1, &_descriptor_set._handle, 0, nullptr);

					const std::array<VkDeviceSize, 1> vertex_buffer_offsets{ 0 };
					vkCmdBindVertexBuffers(command_buffer, 0, 1, &_vertex_buffer.get(), vertex_buffer_offsets.data());

					// TODO: Do actual rendering.
				});
			});
		}
		catch (const VK_Swapchain::OutOfDate&)
		{
			_context.wait_idle();
			_swapchain.reset();
		}
	}

	std::unique_ptr<RenderProgram> VulkanRenderer::create_builtin_program_2d()
	{
		return std::make_unique<VulkanProgram>(*this);
	}

	std::unique_ptr<Mesh> VulkanRenderer::create_mesh(const MeshData& data)
	{
		const auto vertex_buffer_size = data._vertex_data.size() * vertex_format(data._vertex_format)._binding.stride;

		if (Buffer index_data; data.make_uint16_indices(index_data))
		{
			auto result = std::make_unique<VulkanMesh>(_context, vertex_buffer_size, index_data.size(), VK_INDEX_TYPE_UINT16, data._indices.size());
			result->_vertex_buffer.write(data._vertex_data.data(), vertex_buffer_size);
			result->_index_buffer.write(index_data.data(), index_data.size());
			return result;
		}

		const auto index_buffer_size = data._indices.size() * sizeof(uint32_t);
		auto result = std::make_unique<VulkanMesh>(_context, vertex_buffer_size, index_buffer_size, VK_INDEX_TYPE_UINT32, data._indices.size());
		result->_vertex_buffer.write(data._vertex_data.data(), vertex_buffer_size);
		result->_index_buffer.write(data._indices.data(), index_buffer_size);
		return result;
	}

	std::unique_ptr<RenderProgram> VulkanRenderer::create_program(const std::string&, const std::string&)
	{
		return std::make_unique<VulkanProgram>(*this);
	}

	std::unique_ptr<Texture2D> VulkanRenderer::create_texture_2d(const Image& image, Flags<RenderManager::TextureFlag> flags)
	{
		const auto create = [this, flags](const ImageInfo& info, const void* data) -> std::unique_ptr<Texture2D> {
			const auto has_mipmaps = !(flags & RenderManager::TextureFlag::NoMipmaps);
			return std::make_unique<VulkanTexture2D>(*this, _context, info, has_mipmaps, VK_FORMAT_B8G8R8A8_UNORM, data);
		};

		if (image.info().pixel_format() == PixelFormat::Bgra32 && power_of_2_alignment(image.info().stride()) >= 4)
			return create(image.info(), image.data());

		const ImageInfo transformed_info{ image.info().width(), image.info().height(), PixelFormat::Bgra32, image.info().orientation() };
		Buffer buffer{ transformed_info.frame_size() };
		if (!Image::transform(image.info(), image.data(), transformed_info, buffer.data()))
			return {};

		// TODO: Count "slow" textures.
		return create(transformed_info, buffer.data());
	}

	size_t VulkanRenderer::draw_mesh(const Mesh& mesh)
	{
		static_cast<const VulkanMesh&>(mesh).draw(VK_NULL_HANDLE); // TODO: Use actual command buffer.
		return 0;
	}

	void VulkanRenderer::flush_2d(const Buffer&, const Buffer&) noexcept
	{
	}

	RectF VulkanRenderer::map_rect(const RectF& rect, ImageOrientation) const
	{
		return rect;
	}

	void VulkanRenderer::set_program(const RenderProgram*)
	{
	}

	void VulkanRenderer::set_texture(const Texture2D& texture, Flags<Texture2D::Filter>)
	{
		_descriptor_texture_2d = static_cast<const VulkanTexture2D&>(texture).descriptor_image_info();
		_update_descriptors = true;
	}

	void VulkanRenderer::set_viewport_size(const Size&)
	{
	}

	Image VulkanRenderer::take_screenshot(const Size& viewport_size) const
	{
		return Image{ { static_cast<std::size_t>(viewport_size._width), static_cast<std::size_t>(viewport_size._height), PixelFormat::Rgb24, 4, ImageOrientation::XRightYDown } };
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

		vkUpdateDescriptorSets(_context->_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}

	const VulkanVertexFormat& VulkanRenderer::vertex_format(const std::vector<VA>& vas)
	{
		const auto i = std::find_if(_vertex_format_cache.begin(), _vertex_format_cache.end(), [&vas](const auto& format) { return format.first == vas; });
		return i != _vertex_format_cache.end() ? *i->second : *_vertex_format_cache.emplace_back(vas, std::make_unique<VulkanVertexFormat>(vas)).second;
	}
}
