#ifndef _src_gui_layout_h_
#define _src_gui_layout_h_

#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>
#include <yttrium/std_string_view.h>

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

		Widget& add_widget(const StaticString& type, std::string_view name, GuiPropertyLoader&);

		void draw(Renderer&, const Widget* hover_widget, const Widget* click_widget) const;
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
