#ifndef _src_gui_property_h_
#define _src_gui_property_h_

#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/math/vector.h>
#include <yttrium/renderer/texture.h>

#include <memory>
#include <string>
#include <vector>

namespace Yttrium
{
	class GuiPropertyLoader;
	class Renderer;
	class StaticString;
	class TextCapture;
	class TextureFont;
	class TexturedRect;

	struct BackgroundProperty
	{
		Vector4 color{1, 1, 1};
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
		Vector4 color{1, 1, 1};
		unsigned alignment = 0;
		std::vector<TexturedRect> geometry;

		ForegroundProperty();
		~ForegroundProperty();

		void draw(Renderer&) const;
		bool load(const GuiPropertyLoader&);
		void prepare(const std::string&, const RectF&, TextCapture* = nullptr);
	};
}

#endif
