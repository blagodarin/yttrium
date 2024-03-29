// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../backend.h"
#include "buffer.h"
#include "context.h"
#include "wrappers.h"

#include <optional>

namespace Yt
{
	enum class VA;
	class VulkanSwapchain;
	class VulkanVertexFormat;

	class VulkanRenderer final : public RenderBackend
	{
	public:
		explicit VulkanRenderer(const WindowID&);
		~VulkanRenderer() noexcept override;

		void clear() override;
		std::unique_ptr<RenderProgram> create_builtin_program_2d() override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override;
		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(const seir::ImageInfo&, const void*, Flags<RenderManager::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override;
		void flush_2d(const Buffer&, const Buffer&) noexcept override;
		RectF map_rect(const RectF&, ImageOrientation) const override;
		void set_program(const RenderProgram*) override;
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override;
		void set_viewport_size(const Size&) override;
		seir::Image take_screenshot(const Size&) const override;

		VulkanContext& context() noexcept { return _context; }
		void update_uniforms(const void* data, size_t size) { _uniform_buffer.write(data, size); }

	private:
		void update_descriptors();
		const VulkanVertexFormat& vertex_format(const std::vector<VA>&);

	private:
		VulkanContext _context;
		VulkanBuffer _uniform_buffer;
		VK_DescriptorSetLayout _descriptor_set_layout;
		VK_DescriptorPool _descriptor_pool;
		VK_DescriptorSet _descriptor_set;
		VK_PipelineLayout _pipeline_layout;
		std::vector<std::pair<std::vector<VA>, std::unique_ptr<const VulkanVertexFormat>>> _vertex_format_cache;
		std::optional<VkDescriptorImageInfo> _descriptor_texture_2d;
		bool _update_descriptors = false;
		VK_ShaderModule _vertex_shader;
		VK_ShaderModule _fragment_shader;
		VulkanBuffer _vertex_buffer;
		std::unique_ptr<VulkanSwapchain> _swapchain;
	};
}
