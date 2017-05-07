#ifndef _src_gui_property_h_
#define _src_gui_property_h_

#include <yttrium/math/color.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/std/string_view.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class GuiPropertyLoader;
	class Renderer;
	class TextCapture;
	class TextureFont;
	class TexturedRect;

	struct BackgroundProperty
	{
		Color4f color{1, 1, 1};
		std::shared_ptr<const Texture2D> texture;
		Texture2D::Filter texture_filter = Texture2D::TrilinearFilter;
		RectF texture_rect;
		Margins borders;

		BackgroundProperty() = default;

		void draw(Renderer&, const RectF&) const;
		bool load(const GuiPropertyLoader&);
		void update(const GuiPropertyLoader&);
	};

	struct ForegroundProperty
	{
		std::shared_ptr<const TextureFont> font;
		std::shared_ptr<const Texture2D> font_texture;
		float size = 7 / 8.f;
		Color4f color{1, 1, 1};
		unsigned alignment = 0;
		std::vector<TexturedRect> geometry;

		ForegroundProperty();
		~ForegroundProperty();

		void draw(Renderer&) const;
		bool load(const GuiPropertyLoader&);
		void prepare(std::string_view, const RectF&, TextCapture* = nullptr);
	};
}

#endif
