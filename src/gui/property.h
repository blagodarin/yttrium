#ifndef __GUI_PROPERTY_H
#define __GUI_PROPERTY_H

#include <yttrium/margins.h>
#include <yttrium/pointer.h>
#include <yttrium/rect.h>
#include <yttrium/renderer.h>
#include <yttrium/texture_font.h>

namespace Yttrium
{
	class GuiPropertyDumper;
	class GuiPropertyLoader;
	class Renderer;
	class String;

	struct BackgroundProperty
	{
		Vector4f           color;
		Pointer<Texture2D> texture;
		Rect               texture_rect;
		MarginsI           borders;

		BackgroundProperty(): color(1, 1, 1) {}

		void draw(Renderer& renderer, const RectF& rect) const;
		void dump(GuiPropertyDumper& dumper) const;
		bool load(const GuiPropertyLoader& loader);
		void update(const GuiPropertyLoader& loader);
	};

	struct ForegroundProperty
	{
		TextureFont        font;
		Pointer<Texture2D> font_texture;
		Vector2f           size;
		Vector4f           color;

		ForegroundProperty(): color(1, 1, 1) {}

		void draw(Renderer& renderer, const String& text, const Vector2f& origin,
			unsigned alignment, float scale, Renderer::TextCapture* capture = nullptr) const;
		void dump(GuiPropertyDumper& dumper) const;
		bool load(const GuiPropertyLoader& loader);
		void update(const GuiPropertyLoader& loader);
	};
}

#endif // __GUI_PROPERTY_H
