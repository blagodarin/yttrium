#ifndef _src_gui_layout_h_
#define _src_gui_layout_h_

#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class GuiPrivate;
	class GuiPropertyLoader;
	class RectF;
	class RenderContext;
	class Widget;

	class GuiLayout
	{
	public:
		enum class Placement
		{
			Stretch,
			Center,
			Left,
			Right,
		};

		GuiLayout(GuiPrivate&, Placement);

		void set_size(const SizeF& size) { _size = size; }

		Widget& add_widget(std::unique_ptr<Widget>&&);

		void draw(RenderContext&, const Widget* hover_widget, const Widget* click_widget) const;
		void update(const RectF&);

	private:
		GuiPrivate& _gui;
		const Placement _placement;
		SizeF _size;
		Vector2 _scaling{1, 1};
		std::vector<std::unique_ptr<Widget>> _widgets;
	};
}

#endif
