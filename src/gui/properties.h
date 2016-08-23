#ifndef _src_gui_property_h_
#define _src_gui_property_h_

#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/memory/shared_ptr.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class GuiPropertyLoader;
	class Renderer;
	class StaticString;
	class TextCapture;
	class Texture2D;
	class TextureFont;
	class TexturedRect;

	struct BackgroundProperty
	{
		Vector4              color{1, 1, 1};
		SharedPtr<Texture2D> texture;
		RectF                texture_rect;
		Margins              borders;

		void draw(Renderer&, const RectF& rect) const;
		bool load(const GuiPropertyLoader&);
		void update(const GuiPropertyLoader&);
	};

	struct ForegroundProperty
	{
		SharedPtr<TextureFont> font;
		SharedPtr<Texture2D> font_texture;
		float                size = 7 / 8.f;
		Vector4              color{1, 1, 1};
		unsigned             alignment = 0;
		StdVector<TexturedRect> geometry;

		ForegroundProperty(Allocator&);
		~ForegroundProperty();

		void draw(Renderer&) const;
		bool load(const GuiPropertyLoader&);
		void prepare(const StaticString& text, const RectF& rect, TextCapture* = nullptr);
	};
}

#endif
