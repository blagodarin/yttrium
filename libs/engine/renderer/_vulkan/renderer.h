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

#include "../backend.h"
#include "buffer.h"
#include "context.h"
#include "wrappers.h"

#include <optional>

namespace Yttrium
{
	enum class VA;
	class VulkanSwapchain;
	class VulkanVertexFormat;

	class VulkanRenderer final : public RenderBackend
	{
	public:
		explicit VulkanRenderer(const WindowBackend&);
		~VulkanRenderer() noexcept override;

		void clear() override;
		std::unique_ptr<RenderProgram> create_builtin_program_2d() override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override;
		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(const Image&, Flags<RenderManager::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override;
		void flush_2d(const Buffer&, const Buffer&) noexcept override;
		RectF map_rect(const RectF&, ImageOrientation) const override;
		void set_program(const RenderProgram*) override;
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override;
		void set_window_size(const Size&) override;
		Image take_screenshot(const Size&) const override;

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
