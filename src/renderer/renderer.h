#ifndef _src_renderer_renderer_h_
#define _src_renderer_renderer_h_

#include <yttrium/renderer.h>

#include <yttrium/math/margins.h>
#include <yttrium/memory/buffer.h>
#include <yttrium/object.h>
#include <yttrium/pointer.h>
#include <yttrium/std/vector.h>
#include <yttrium/string.h>
#include <yttrium/texture_font.h>
#include "../base/private_base.h"

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

		static Pointer<RendererImpl> create(WindowBackend&, Allocator&);

		RendererImpl(Allocator&);
		~RendererImpl() override;

		Matrix4 current_projection() const override;
		Matrix4 current_transformation() const override;
		void draw_rectangle(const RectF&) override;
		void draw_rectangle(const RectF&, const RectF& texture_rect) override;
		void draw_text(const PointF&, float, const StaticString&, TextCapture*) override;
		void set_color(const Vector4&) override;
		bool set_font(const TextureFont&) override;
		bool set_texture_borders(const Margins&) override;
		void set_texture_rectangle(const RectF&) override;
		Size window_size() const override { return _window_size; }

		virtual void clear() = 0;
		virtual void take_screenshot(Image&) = 0;

		Allocator& allocator() const { return _allocator; }
		const Texture2D* debug_texture() const;
		void forget_program(const GpuProgram*);
		void forget_texture(const Texture2D*);
		void pop_program();
		void pop_projection();
		void pop_texture();
		void pop_transformation();
		const GpuProgram* program_2d() const { return _program_2d.get(); }
		void push_program(const GpuProgram*);
		void push_projection_2d(const Matrix4&);
		void push_projection_3d(const Matrix4&);
		void push_texture(const Texture2D*);
		void push_transformation(const Matrix4&);
		Statistics reset_statistics();
		void set_window_size(const Size&);

	protected:

		struct Vertex2D
		{
			Vector2 position;
			Vector4 color;
			Vector2 texture;
		};

		virtual void flush_2d_impl(const Buffer& vertices, const Buffer& indices) = 0;
		virtual void set_program(const GpuProgram*) = 0;
		virtual void set_texture(const Texture2D&) = 0;
		virtual void set_window_size_impl(const Size&) = 0;

		const BackendTexture2D* current_texture_2d() const;
		void update_state();

	private:

		void draw_rectangle(const RectF& position, const RectF& texture, const MarginsF& borders);
		void flush_2d();
		void reset_texture_state();

	protected:

		Statistics _statistics;

	private:

		Allocator& _allocator;

		Size _window_size;

		Vector4 _color;

		Buffer _vertices_2d;
		Buffer _indices_2d;

		RectF    _texture_rect;
		MarginsF _texture_borders;

		TextureFont _font;

		SharedPtr<Texture2D> _white_texture;
		SharedPtr<Texture2D> _debug_texture;
		Pointer<GpuProgram> _program_2d;

		enum class MatrixType
		{
			Projection,
			Transformation,
		};

		StdVector<std::pair<Matrix4, MatrixType>> _matrix_stack;

		StdVector<std::pair<const Texture2D*, int>> _texture_stack;
		const Texture2D* _current_texture = nullptr;
		bool _reset_texture = false;
#if Y_IS_DEBUG
		StdVector<const Texture2D*> _seen_textures; // For redundancy statistics.
#endif

		StdVector<std::pair<const GpuProgram*, int>> _program_stack;
		const GpuProgram* _current_program = nullptr;
		bool _reset_program = false;
#if Y_IS_DEBUG
		StdVector<const GpuProgram*> _seen_programs; // For redundancy statistics.
#endif
	};
}

#endif
