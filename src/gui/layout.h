#ifndef _src_gui_layout_h_
#define _src_gui_layout_h_

#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class GuiImpl;
	class GuiPropertyLoader;
	class RectF;
	class Renderer;
	class StaticString;
	class Widget;
	template <typename> class Pointer;

	class GuiLayout
	{
	public:
		enum class Placement
		{
			Stretch,
			Center,
		};

		GuiLayout(const GuiImpl&, Placement);

		void set_size(const SizeF& size) { _size = size; }

		Widget* add_widget(const StaticString& type, GuiPropertyLoader&);

		void update(const RectF&);
		void render(Renderer&, const Widget* hover_widget, const Widget* click_widget) const;

	private:
		const GuiImpl& _gui;
		const Placement _placement;
		SizeF _size;
		Vector2 _scaling{1, 1};
		StdVector<Pointer<Widget>> _widgets;
	};
}

#endif
