#ifndef _src_gui_property_h_
#define _src_gui_property_h_

#include <yttrium/math/color.h>
#include <yttrium/math/margins.h>
#include <yttrium/math/rect.h>
#include <yttrium/renderer/texture.h>
#include <yttrium/std/optional.h>
#include "actions.h"

#include <map>

namespace Yttrium
{
	class Renderer;
	class Sound;
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
		void prepare(std::string_view, const RectF&, TextCapture* = nullptr);
	};

	struct WidgetData
	{
		enum class Style
		{
			Normal,
			Hovered,
			Pressed,
			Checked,
			Disabled,
		};

		struct StyleData
		{
			ForegroundProperty _foreground;
			BackgroundProperty _background;
		};

		enum class Action
		{
			OnClick,
			OnUpdate,
			OnEnter,
		};

		RectF _rect{{0, 0}, SizeF{0, 0}};
		std::shared_ptr<const Sound> _sound;
		std::map<Style, StyleData> _styles;
		std::map<Action, GuiActions> _actions;
		std::optional<Style> _fixed_style;
		std::string _text;

		WidgetData();
		WidgetData(const WidgetData&); // TODO: Fix GuiActions non-copyability.

		void run(GuiPrivate&, Action) const;
		StyleData& style_data(Style);
	};
}

#endif
