/// \file
/// \brief

#ifndef _include_yttrium_renderer_modifiers_h_
#define _include_yttrium_renderer_modifiers_h_

#include <yttrium/global.h>

namespace Yttrium
{
	class GpuProgram;
	class Matrix4;
	class Renderer;
	class Texture2D;

	/// Base class for Renderer modifiers.
	class RendererModifier
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
		Push3D(Renderer&, const Matrix4&);

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
		PushTexture(Renderer&, const Texture2D*);

		///
		~PushTexture();
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
