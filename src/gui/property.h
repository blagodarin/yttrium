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
		Rect                 texture_rect;
		Margins              borders;

		void draw(Renderer& renderer, const RectF& rect) const;
		bool load(const GuiPropertyLoader& loader);
		void update(const GuiPropertyLoader& loader);
	};

	struct ForegroundProperty
	{
		TextureFont          font;
		SharedPtr<Texture2D> font_texture;
		SizeF                size;
		Vector4              color{1, 1, 1};

		void draw(Renderer& renderer, const String& text, const PointF& origin,
			unsigned alignment, float scale, Renderer::TextCapture* capture = nullptr) const;
		bool load(const GuiPropertyLoader& loader);
		void update(const GuiPropertyLoader& loader);
	};
}

#endif
