#ifndef _src_gui_widgets_label_h_
#define _src_gui_widgets_label_h_

#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class LabelWidget : public Widget
	{
	public:
		explicit LabelWidget(GuiPrivate& gui) : Widget{gui} {}

		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		std::string _text;
		mutable ForegroundProperty _foreground;
		mutable std::string _final_text;
	};
}

#endif
