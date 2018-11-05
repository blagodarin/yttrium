#ifndef _include_yttrium_renderer_modifiers_h_
#define _include_yttrium_renderer_modifiers_h_

#include <yttrium/api.h>
#include <yttrium/renderer/texture.h>

#include <string>

namespace Yttrium
{
	class Material;
	class Matrix4;
	class RenderPass;
	class RenderProgram;
	class SizeF;

	/// Base class for RenderPass modifiers.
	class Y_ENGINE_API RenderModifier
	{
	public:
		RenderModifier(const RenderModifier&) = delete;
		RenderModifier& operator=(const RenderModifier&) = delete;
	protected:
		RenderPass& _pass;
		explicit RenderModifier(RenderPass& pass) noexcept : _pass{pass} {}
	};

	///
	class Y_ENGINE_API Push2D : public RenderModifier
	{
	public:
		///
		explicit Push2D(RenderPass&);

		///
		~Push2D() noexcept;
	};

	///
	class Y_ENGINE_API Push3D : public RenderModifier
	{
	public:
		///
		Push3D(RenderPass&, const Matrix4& projection, const Matrix4& view);

		///
		~Push3D() noexcept;
	};

	///
	class Y_ENGINE_API PushProgram : public RenderModifier
	{
	public:
		///
		PushProgram(RenderPass&, const RenderProgram*);

		///
		~PushProgram() noexcept;
	};

	///
	class Y_ENGINE_API PushTexture : public RenderModifier
	{
	public:
		///
		PushTexture(RenderPass&, const Texture2D*, Flags<Texture2D::Filter> = Texture2D::NearestFilter);

		///
		~PushTexture() noexcept;

	private:
		const Flags<Texture2D::Filter> _filter;
	};

	///
	class Y_ENGINE_API PushMaterial : public RenderModifier
	{
	public:
		///
		PushMaterial(RenderPass&, const Material*);

		///
		void set_uniform(const std::string&, const Matrix4&);

	private:
		const Material* const _material;
		PushProgram _program;
		PushTexture _texture;
	};

	///
	class Y_ENGINE_API PushTransformation : public RenderModifier
	{
	public:
		/// Multiplies the current transformation matrix by the specified one,
		/// pushes it to the transformation stack and applies the resulting transformation.
		PushTransformation(RenderPass&, const Matrix4&);

		/// Pops a matrix from the transformation stack and applies the previous matrix.
		~PushTransformation() noexcept;
	};
}

#endif
