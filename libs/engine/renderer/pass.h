#ifndef _src_renderer_pass_h_
#define _src_renderer_pass_h_

#include <yttrium/renderer/pass.h>

#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/utils/flags.h>

#include <string>

namespace Yttrium
{
	class BackendTexture2D;
	class Matrix4;
	class Quad;
	class RenderBackend;
	class RenderBuiltin;
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
		Buffer _vertices_2d;
		Buffer _indices_2d;
		std::vector<std::pair<Matrix4, RenderMatrixType>> _matrix_stack;
		std::vector<std::pair<const Texture2D*, int>> _texture_stack{ { nullptr, 1 } };
#ifndef NDEBUG
		std::vector<const Texture2D*> _seen_textures; // For redundancy statistics.
#endif
		std::vector<std::pair<const RenderProgram*, int>> _program_stack{ { nullptr, 1 } };
#ifndef NDEBUG
		std::vector<const RenderProgram*> _seen_programs; // For redundancy statistics.
#endif
		std::string _debug_text;
		friend class RenderPassImpl;
	};

	class RenderPassImpl : public RenderPass
	{
	public:
		RenderPassImpl(RenderBackend&, RenderBuiltin&, RenderPassData&, const Size& window_size);
		~RenderPassImpl() noexcept override;

		void add_debug_text(std::string_view) override;
		void draw_mesh(const Mesh&) override;
		void draw_quad(const Quad&, const Color4f&) override;
		void draw_rect(const RectF&, const Color4f&) override;
		void draw_rects(const std::vector<TexturedRect>&, const Color4f&) override;
		Matrix4 full_matrix() const override;
		Matrix4 model_matrix() const override;
		Line3 pixel_ray(const Vector2&) const override;
		void set_texture_rect(const RectF&, const MarginsF&) override;
		SizeF window_size() const override;

	public:
		struct Statistics
		{
			size_t _triangles = 0;
			size_t _draw_calls = 0;
			size_t _texture_switches = 0;
			size_t _redundant_texture_switches = 0;
			size_t _shader_switches = 0;
			size_t _redundant_shader_switches = 0;
		};

		RenderBuiltin& builtin() const noexcept { return _builtin; }
		void draw_debug_text();
		void draw_rect(const RectF& position, const Color4f&, const RectF& texture);
		void pop_program() noexcept;
		void pop_projection() noexcept;
		void pop_texture(Flags<Texture2D::Filter>) noexcept;
		void pop_transformation() noexcept;
		void push_program(const RenderProgram*);
		void push_projection_2d(const Matrix4&);
		void push_projection_3d(const Matrix4& projection, const Matrix4& view);
		Flags<Texture2D::Filter> push_texture(const Texture2D*, Flags<Texture2D::Filter>);
		void push_transformation(const Matrix4&);
		Statistics statistics() const noexcept { return _statistics; }

	private:
		struct Batch2D;

		const BackendTexture2D* current_texture_2d() const;
		void draw_rect(const RectF& position, const Color4f&, const RectF& texture, const MarginsF& borders);
		void flush_2d() noexcept;
		Batch2D prepare_batch_2d(std::size_t vertex_count, std::size_t index_count);
		void reset_texture_state();
		void update_state();

	private:
		RenderBackend& _backend;
		RenderBuiltin& _builtin;
		RenderPassData& _data;
		const SizeF _window_size;

		const Texture2D* _current_texture = nullptr;
		Flags<Texture2D::Filter> _current_texture_filter = Texture2D::NearestFilter;
		RectF _texture_rect;
		MarginsF _texture_borders;
		bool _reset_texture = false;

		const RenderProgram* _current_program = nullptr;
		bool _reset_program = false;

		Statistics _statistics;
	};
}

#endif
