#include "label.h"

#include <yttrium/log.h>
#include <yttrium/renderer.h>
#include <yttrium/script/context.h>
#include "../gui.h"
#include "../property_dumper.h"
#include "../property_loader.h"

// TODO: Use proper rects for labels, scaling the text accordingly.

namespace Yttrium
{
	LabelWidget::LabelWidget(const GuiImpl& gui)
		: Widget(gui)
		, _final_text(&gui.allocator())
	{
	}

	void LabelWidget::dump(GuiPropertyDumper& dumper) const
	{
		dumper.dump_position("position"_s, _position);
		dumper.dump_scaling("scale"_s, _scaling);
		_foreground.dump(dumper);
		dumper.dump_alignment("align"_s, _alignment);
		dumper.dump_text("text"_s, _text);
	}

	bool LabelWidget::load(GuiPropertyLoader& loader)
	{
		if (!(loader.load_position("position"_s, &_position)
			&& _foreground.load(loader)
			&& loader.load_translatable("text"_s, &_text)))
		{
			Log() << "(gui/label) Unable to load"_s;
			return false;
		}

		loader.load_scaling("scale"_s, &_scaling);
		loader.load_alignment("align"_s, &_alignment);

		_rect = RectF(_position, {});

		return true;
	}

	void LabelWidget::render(Renderer& renderer, const RectF& rect, const Vector2& scale, WidgetState) const
	{
		_gui.script_context().substitute(_final_text, _text);

		const auto& text_size = _foreground.font.text_size(_final_text, _foreground.size) * scale;

		RectF final_rect = rect;
		if ((_alignment & HorizontalAlignmentMask) != RightAlignment)
			final_rect.set_left(final_rect.left() - text_size.x * (_alignment & LeftAlignment ? 1.0 : 0.5));
		if ((_alignment & VerticalAlignmentMask) != BottomAlignment)
			final_rect.set_top(final_rect.top() - text_size.y * (_alignment & TopAlignment ? 1.0 : 0.5));
		final_rect.set_size(text_size);

		_foreground.draw(renderer, _final_text, final_rect.center(), 0, scale.y);
	}
}
