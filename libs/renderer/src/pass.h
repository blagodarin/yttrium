// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/pass.h>

#include <yttrium/geometry/margins.h>
#include <yttrium/geometry/rect.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/utils/flags.h>

#include <string>

namespace Yt
{
	class BackendTexture2D;
	class Matrix4;
	class Quad;
	class RenderBackend;
	class RenderBuiltin;
	class RenderMetrics;
	class RenderProgram;

	enum class RenderMatrixType
	{
		Projection,
		View,
		Model,
	};

	// Data that persists between frames.
	class RenderPassData
	{
	public:
		RenderPassData();
		~RenderPassData() noexcept;

	private:
		std::vector<std::pair<Matrix4, RenderMatrixType>> _matrix_stack;
		std::vector<std::pair<const Texture2D*, int>> _texture_stack{ { nullptr, 1 } };
#ifndef NDEBUG
		std::vector<const Texture2D*> _seen_textures; // For redundancy statistics.
#endif
		std::vector<std::pair<const RenderProgram*, int>> _program_stack{ { nullptr, 1 } };
#ifndef NDEBUG
		std::vector<const RenderProgram*> _seen_programs; // For redundancy statistics.
#endif
		friend class RenderPassImpl;
	};

	class RenderPassImpl : public RenderPass
	{
	public:
		RenderPassImpl(RenderBackend&, RenderBuiltin&, RenderPassData&, const Size& viewport_size, RenderMetrics&);
		~RenderPassImpl() noexcept override;

		void draw_mesh(const Mesh&) override;
		Matrix4 full_matrix() const override;
		Matrix4 model_matrix() const override;
		Line3 pixel_ray(const Vector2&) const override;
		RectF viewport_rect() const override;

	public:
		RenderBuiltin& builtin() const noexcept { return _builtin; }
		void flush_2d(const Buffer& vertices, const Buffer& indices) noexcept;
		void pop_program() noexcept;
		void pop_projection() noexcept;
		void pop_texture(Flags<Texture2D::Filter>) noexcept;
		void pop_transformation() noexcept;
		void push_program(const RenderProgram*);
		void push_projection_2d(const Matrix4&);
		void push_projection_3d(const Matrix4& projection, const Matrix4& view);
		Flags<Texture2D::Filter> push_texture(const Texture2D*, Flags<Texture2D::Filter>);
		void push_transformation(const Matrix4&);

	private:
		void update_state();

	private:
		RenderBackend& _backend;
		RenderBuiltin& _builtin;
		RenderPassData& _data;
		const SizeF _viewport_size;
		RenderMetrics& _metrics;

		const Texture2D* _current_texture = nullptr;
		Flags<Texture2D::Filter> _current_texture_filter = Texture2D::NearestFilter;
		bool _reset_texture = false;

		const RenderProgram* _current_program = nullptr;
		bool _reset_program = false;
	};
}
