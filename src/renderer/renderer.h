#ifndef _src_renderer_renderer_h_
#define _src_renderer_renderer_h_

#include <yttrium/renderer/renderer.h>

#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/renderer/texture.h>

#include <memory>

namespace Yttrium
{
	class BackendTexture2D;
	enum class ImageOrientation;
	class RendererBackend;
	class WindowBackend;

	class RendererImpl final : public Renderer, public RenderContext
	{
	public:
		explicit RendererImpl(WindowBackend&);
		~RendererImpl() override;

		void add_debug_text(std::string_view) override;
		std::unique_ptr<GpuProgram> create_gpu_program(const std::string& vertex_shader, const std::string& fragment_shader) override;
		std::unique_ptr<Texture2D> create_texture_2d(Image&&, Flags<TextureFlag>) override;
		void draw_mesh(const Mesh&) override;
		void draw_quad(const Quad&, const Color4f&) override;
		void draw_rect(const RectF&, const Color4f&) override;
		void draw_rects(const std::vector<TexturedRect>&, const Color4f&) override;
		Matrix4 full_matrix() const override;
		std::unique_ptr<Mesh> load_mesh(const Source&) override;
		Matrix4 model_matrix() const override;
		Line3 pixel_ray(const Vector2&) const override;
		void set_texture_rect(const RectF&, const MarginsF&) override;
		SizeF window_size() const override { return SizeF{_window_size}; }

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

		void clear();
		const Texture2D* debug_texture() const;
		void draw_rect(const RectF& position, const Color4f&, const RectF& texture);
		void finish();
		void forget_program(const GpuProgram*);
		void forget_texture(const Texture2D*);
		RectF map_rect(const RectF&, ImageOrientation) const;
		void pop_program();
		void pop_projection();
		void pop_texture(Flags<Texture2D::Filter>);
		void pop_transformation();
		const GpuProgram* program_2d() const { return _program_2d.get(); }
		void push_program(const GpuProgram*);
		void push_projection_2d(const Matrix4&);
		void push_projection_3d(const Matrix4& projection, const Matrix4& view);
		Flags<Texture2D::Filter> push_texture(const Texture2D*, Flags<Texture2D::Filter>);
		void push_transformation(const Matrix4&);
		Statistics reset_statistics();
		void set_window_size(const Size&);
		Image take_screenshot() const;

	private:
		const BackendTexture2D* current_texture_2d() const;
		void draw_rect(const RectF& position, const Color4f&, const RectF& texture, const MarginsF& borders);
		void flush_2d();
		void reset_texture_state();
		void update_state();

	private:
		const std::unique_ptr<RendererBackend> _backend;

		Statistics _statistics;

		Size _window_size;

		Buffer _vertices_2d;
		Buffer _indices_2d;

		RectF _texture_rect;
		MarginsF _texture_borders;

		std::unique_ptr<const Texture2D> _white_texture;
		std::unique_ptr<const Texture2D> _debug_texture;
		std::unique_ptr<GpuProgram> _program_2d;

		enum class MatrixType
		{
			Projection,
			View,
			Model,
		};

		std::vector<std::pair<Matrix4, MatrixType>> _matrix_stack;

		std::vector<std::pair<const Texture2D*, int>> _texture_stack{ { nullptr, 1 } };
		const Texture2D* _current_texture = nullptr;
		Flags<Texture2D::Filter> _current_texture_filter = Texture2D::NearestFilter;
		bool _reset_texture = false;
#ifndef NDEBUG
		std::vector<const Texture2D*> _seen_textures; // For redundancy statistics.
#endif

		std::vector<std::pair<const GpuProgram*, int>> _program_stack{ { nullptr, 1 } };
		const GpuProgram* _current_program = nullptr;
		bool _reset_program = false;
#ifndef NDEBUG
		std::vector<const GpuProgram*> _seen_programs; // For redundancy statistics.
#endif

		std::string _debug_text;
	};
}

#endif
