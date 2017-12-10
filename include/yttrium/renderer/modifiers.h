/// \file
/// \brief

#ifndef _include_yttrium_renderer_modifiers_h_
#define _include_yttrium_renderer_modifiers_h_

#include <yttrium/api.h>
#include <yttrium/renderer/texture.h>

#include <string>

namespace Yttrium
{
	class GpuProgram;
	class Material;
	class Matrix4;
	class RenderContext;
	class SizeF;

	/// Base class for RenderContext modifiers.
	class Y_API RenderModifier
	{
	public:
		RenderModifier(const RenderModifier&) = delete;
		RenderModifier& operator=(const RenderModifier&) = delete;
	protected:
		RenderContext& _context;
		explicit RenderModifier(RenderContext& context) noexcept : _context{context} {}
	};

	///
	class Y_API Push2D : public RenderModifier
	{
	public:
		///
		explicit Push2D(RenderContext&);

		///
		~Push2D() noexcept;
	};

	///
	class Y_API Push3D : public RenderModifier
	{
	public:
		///
		Push3D(RenderContext&, const Matrix4& projection, const Matrix4& view);

		///
		~Push3D() noexcept;
	};

	///
	class Y_API PushGpuProgram : public RenderModifier
	{
	public:
		///
		PushGpuProgram(RenderContext&, const GpuProgram*);

		///
		~PushGpuProgram() noexcept;
	};

	///
	class Y_API PushTexture : public RenderModifier
	{
	public:
		///
		PushTexture(RenderContext&, const Texture2D*, Flags<Texture2D::Filter> = Texture2D::NearestFilter);

		///
		~PushTexture() noexcept;

	private:
		const Flags<Texture2D::Filter> _filter;
	};

	///
	class Y_API PushMaterial : public RenderModifier
	{
	public:
		///
		PushMaterial(RenderContext&, const Material*);

		///
		void set_uniform(const std::string&, const Matrix4&);

	private:
		const Material* const _material;
		PushGpuProgram _gpu_program;
		PushTexture _texture;
	};

	///
	class Y_API PushTransformation : public RenderModifier
	{
	public:
		/// Multiplies the current transformation matrix by the specified one,
		/// pushes it to the transformation stack and applies the resulting transformation.
		PushTransformation(RenderContext&, const Matrix4&);

		/// Pops a matrix from the transformation stack and applies the previous matrix.
		~PushTransformation() noexcept;
	};
}

#endif
