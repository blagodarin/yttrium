#include "label.h"

#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"
#include "../property_loader.h"

// TODO: Use proper rects for labels, scaling the text accordingly.

namespace Yttrium
{
	LabelWidget::LabelWidget(const GuiImpl& gui)
		: Widget(gui)
		, _final_text(&gui.allocator())
	{
	}

	bool LabelWidget::load(GuiPropertyLoader& loader)
	{
		if (!(loader.load_position("position"_s, _position)
			&& _foreground.load(loader)
			&& loader.load_translatable("text"_s, &_text)))
		{
			Log() << "(gui/label) Unable to load"_s;
			return false;
		}

		loader.load_alignment("align"_s, &_alignment);

		_rect = RectF(_position, SizeF(1, 1));

		return true;
	}

	void LabelWidget::render(Renderer& renderer, const RectF& rect, const Vector2& scale, WidgetState) const
	{
		_gui.script_context().substitute(_final_text, _text);

		const auto& text_size = _foreground.font.text_size(_final_text, _foreground.size) * std::make_pair(scale.x, scale.y);

		auto x = rect.left();
		if ((_alignment & HorizontalAlignmentMask) != RightAlignment)
			x -= text_size.width() * (_alignment & LeftAlignment ? 1.0 : 0.5);

		auto y = rect.top();
		if ((_alignment & VerticalAlignmentMask) != BottomAlignment)
			y -= text_size.height() * (_alignment & TopAlignment ? 1.0 : 0.5);

		_foreground.draw(renderer, _final_text, RectF({x, y}, text_size).center(), 0, scale.y);
	}
}
