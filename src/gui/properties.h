#ifndef _src_gui_property_h_
#define _src_gui_property_h_

#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/object.h>
#include <yttrium/renderer.h>
#include <yttrium/texture_font.h>

namespace Yttrium
{
	class GuiPropertyLoader;
	class Renderer;
	class String;

	struct BackgroundProperty
	{
		Vector4              color{1, 1, 1};
		SharedPtr<Texture2D> texture;
		RectF                texture_rect;
		Margins              borders;

		void draw(Renderer& renderer, const RectF& rect) const;
		bool load(const GuiPropertyLoader& loader);
		void update(const GuiPropertyLoader& loader);
	};

	struct ForegroundProperty
	{
		TextureFont          font;
		SharedPtr<Texture2D> font_texture;
		float                size = .875f;
		Vector4              color{1, 1, 1};
		unsigned             alignment = 0;

		void draw(Renderer& renderer, const StaticString& text, const RectF& rect, Renderer::TextCapture* capture = nullptr) const;
		bool load(const GuiPropertyLoader& loader);
		void update(const GuiPropertyLoader& loader);
	};
}

#endif
