#ifndef _src_gui_widgets_label_h_
#define _src_gui_widgets_label_h_

#include <yttrium/string.h>
#include "../properties.h"
#include "widget.h"

namespace Yttrium
{
	class LabelWidget : public Widget
	{
	public:
		LabelWidget(GuiPrivate&);

		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, WidgetState) const override;

	private:
		String _text;
		mutable ForegroundProperty _foreground;
		mutable String _final_text;
	};
}

#endif
