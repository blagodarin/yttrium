/// \file
/// \brief

#ifndef _include_yttrium_renderer_modifiers_h_
#define _include_yttrium_renderer_modifiers_h_

#include <yttrium/renderer/texture.h>

#include <string>

namespace Yttrium
{
	class GpuProgram;
	class Material;
	class Matrix4;
	class Renderer;

	/// Base class for Renderer modifiers.
	class Y_API RendererModifier
	{
	public:
		RendererModifier(const RendererModifier&) = delete;
		RendererModifier& operator=(const RendererModifier&) = delete;
	protected:
		Renderer& _renderer;
		RendererModifier(Renderer& renderer) : _renderer(renderer) {}
	};

	///
	class Y_API Push2D : public RendererModifier
	{
	public:
		///
		Push2D(Renderer&);

		///
		~Push2D();
	};

	///
	class Y_API Push3D : public RendererModifier
	{
	public:
		///
		Push3D(Renderer&, const Matrix4& projection, const Matrix4& view);

		///
		~Push3D();
	};

	///
	class Y_API PushGpuProgram : public RendererModifier
	{
	public:
		///
		PushGpuProgram(Renderer&, const GpuProgram*);

		///
		~PushGpuProgram();
	};

	///
	class Y_API PushTexture : public RendererModifier
	{
	public:
		///
		PushTexture(Renderer&, const Texture2D*, Texture2D::Filter = Texture2D::NearestFilter);

		///
		~PushTexture();

	private:
		const Texture2D::Filter _filter;
	};

	///
	class Y_API PushMaterial : public RendererModifier
	{
	public:
		///
		PushMaterial(Renderer&, const Material*);

		///
		void set_uniform(const std::string&, const Matrix4&);

	private:
		const Material* const _material;
		PushGpuProgram _gpu_program;
		PushTexture _texture;
	};

	///
	class Y_API PushTransformation : public RendererModifier
	{
	public:
		/// Multiplies the current transformation matrix by the specified one,
		/// pushes it to the transformation stack and applies the resulting transformation.
		PushTransformation(Renderer&, const Matrix4&);

		/// Pops a matrix from the transformation stack and applies the previous matrix.
		~PushTransformation();
	};
}

#endif
