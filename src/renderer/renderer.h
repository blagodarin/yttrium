#ifndef _src_renderer_renderer_h_
#define _src_renderer_renderer_h_

#include <yttrium/renderer.h>

#include <yttrium/margins.h>
#include <yttrium/object.h>
#include <yttrium/pointer.h>
#include <yttrium/string.h>
#include <yttrium/texture_font.h>
#include "../base/private_base.h"

#include <vector>

namespace Yttrium
{
	class BackendTexture2D;
	class Image;
	class WindowBackend;

	class RendererImpl : public Renderer
	{
	public:

		struct Statistics
		{
			int _triangles = 0;
			int _draw_calls = 0;
			int _texture_switches = 0;
			int _redundant_texture_switches = 0;
			int _shader_switches = 0;
			int _redundant_shader_switches = 0;
		};

		static Pointer<RendererImpl> create(WindowBackend& window, Allocator* allocator);

		RendererImpl(Allocator* allocator);
		~RendererImpl() override;

		void draw_rectangle(const RectF& rect) override;
		void draw_rectangle(const RectF& rect, const RectF& texture_rect) override;
		void draw_text(const Vector2& position, const StaticString& text, unsigned alignment, TextCapture* capture) override;
		void set_color(const Vector4& color) override;
		bool set_font(const TextureFont& font) override;
		void set_font_size(const Vector2& size) override;
		bool set_texture_borders(const Margins& borders) override;
		void set_texture_rectangle(const RectF& rect) override;
		Vector2 text_size(const StaticString& text) const override;
		Size window_size() const override { return _window_size; }

		virtual void clear() = 0;
		virtual void take_screenshot(Image& image) = 0;

		Allocator* allocator() const { return _allocator; }
		const Texture2D* debug_texture() const;
		void forget_program(const GpuProgram* program);
		void forget_texture(const Texture2D* texture);
		void pop_program();
		void pop_projection();
		void pop_texture();
		void pop_transformation();
		const GpuProgram* program_2d() const { return _program_2d.get(); }
		void push_program(const GpuProgram* program);
		void push_projection_2d(const Matrix4& matrix);
		void push_projection_3d(const Matrix4& matrix);
		void push_texture(const Texture2D* texture);
		void push_transformation(const Matrix4& matrix);
		Statistics reset_statistics();
		void set_window_size(const Size& size);

	protected:

		struct Vertex2D
		{
			Vector2 position;
			Vector4 color;
			Vector2 texture;
		};

		virtual void flush_2d_impl(const std::vector<Vertex2D>& vertices, const std::vector<uint16_t>& indices) = 0;
		virtual bool initialize() = 0;
		virtual void set_program(const GpuProgram*) = 0;
		virtual void set_projection(const Matrix4&) = 0;
		virtual void set_texture(const Texture2D*) = 0;
		virtual void set_transformation(const Matrix4&) = 0;
		virtual void set_window_size_impl(const Size& size) = 0;

		const BackendTexture2D* current_texture_2d() const;
		void update_state();

	private:

		void draw_rectangle(const RectF& position, const RectF& texture, const MarginsF& borders);
		void flush_2d();
		void reset_texture_state();

	protected:

		Statistics _statistics;

	private:

		Allocator* const _allocator;

		Size _window_size;

		Vector4 _color;

		std::vector<Vertex2D> _vertices_2d;
		std::vector<uint16_t> _indices_2d;

		RectF    _texture_rect;
		MarginsF _texture_borders;

		TextureFont _font;
		Vector2     _font_size;

		SharedPtr<Texture2D> _white_texture;
		SharedPtr<Texture2D> _debug_texture;
		Pointer<GpuProgram> _program_2d;

		enum class MatrixType
		{
			Projection,
			Transformation,
		};

		std::vector<std::pair<Matrix4, MatrixType>> _matrix_stack;

		std::vector<std::pair<const Texture2D*, int>> _texture_stack = {{nullptr, 1}};
		const Texture2D* _current_texture = nullptr;
		bool _reset_texture = false;
#if Y_IS_DEBUG
		std::vector<const Texture2D*> _seen_textures; // For redundancy statistics.
#endif

		std::vector<std::pair<const GpuProgram*, int>> _program_stack = {{nullptr, 1}};
		const GpuProgram* _current_program = nullptr;
		bool _reset_program = false;
#if Y_IS_DEBUG
		std::vector<const GpuProgram*> _seen_programs; // For redundancy statistics.
#endif
	};
}

#endif
