#ifndef _src_gui_layout_h_
#define _src_gui_layout_h_

#include <yttrium/math/size.h>
#include <yttrium/math/vector2.h>

#include <memory>
#include <vector>

namespace Yttrium
{
	class GuiPrivate;
	class GuiPropertyLoader;
	class RectF;
	class Renderer;
	class StaticString;
	class Widget;

	class GuiLayout
	{
	public:
		enum class Placement
		{
			Stretch,
			Center,
		};

		GuiLayout(GuiPrivate&, Placement);

		void set_size(const SizeF& size) { _size = size; }

		Widget& add_widget(const StaticString& type, GuiPropertyLoader&);

		void update(const RectF&);
		void render(Renderer&, const Widget* hover_widget, const Widget* click_widget) const;

	private:
		GuiPrivate& _gui;
		const Placement _placement;
		SizeF _size;
		Vector2 _scaling{1, 1};
		std::vector<std::unique_ptr<Widget>> _widgets;
	};
}

#endif
