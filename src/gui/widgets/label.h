#ifndef _src_gui_widgets_label_h_
#define _src_gui_widgets_label_h_

#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class LabelWidget : public Widget
	{
	public:
		LabelWidget(const GuiImpl&);

		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:
		mutable ForegroundProperty _foreground;
		mutable String _final_text;
	};
}

#endif
