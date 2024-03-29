// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../backend.h"
#include "wrappers.h"

namespace Yt
{
	struct WindowID;

	class GlRenderer final : public RenderBackend
	{
	public:
		explicit GlRenderer(const WindowID&);
		~GlRenderer() noexcept override;

		void clear() override;
		std::unique_ptr<RenderProgram> create_builtin_program_2d() override;
		std::unique_ptr<Mesh> create_mesh(const MeshData&) override;
		std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(const seir::ImageInfo&, const void*, Flags<RenderManager::TextureFlag>) override;
		size_t draw_mesh(const Mesh&) override;
		void flush_2d(const Buffer&, const Buffer&) noexcept override;
		seir::RectF map_rect(const seir::RectF&, seir::ImageAxes) const override;
		void set_program(const RenderProgram*) override;
		void set_texture(const Texture2D&, Flags<Texture2D::Filter>) override;
		void set_viewport_size(const seir::Size&) override;
		seir::Image take_screenshot(const seir::Size&) const override;

	private:
#ifndef NDEBUG
		void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) const;
#endif

	private:
		const GlApi _gl;
		GlBufferHandle _2d_ibo{ _gl, GL_ELEMENT_ARRAY_BUFFER };
		GlBufferHandle _2d_vbo{ _gl, GL_ARRAY_BUFFER };
		GlVertexArrayHandle _2d_vao{ _gl };
	};
}
