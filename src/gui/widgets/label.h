#ifndef __GUI_WIDGETS_LABEL_H
#define __GUI_WIDGETS_LABEL_H

#include "../property.h"
#include "widget.h"

namespace Yttrium
{
	class Label: public Widget
	{
	public:

		Label(Allocator* allocator) : Widget(allocator), _final_text(allocator) {}

		void dump(GuiPropertyDumper&) const override;
		bool load(GuiPropertyLoader&) override;
		void render(Renderer&, const RectF&, const Vector2&, WidgetState) const override;

	private:

		Vector2            _position;
		ForegroundProperty _foreground;
		unsigned           _alignment = 0;
		mutable String     _final_text;
	};
}

#endif // __GUI_WIDGETS_LABEL_H
