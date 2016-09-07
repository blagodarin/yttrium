#ifndef _src_gui_layout_h_
#define _src_gui_layout_h_

#include <yttrium/math/size.h>
#include <yttrium/math/vector.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class GuiPrivate;
	class GuiPropertyLoader;
	class RectF;
	class Renderer;
	class StaticString;
	class Widget;

	template <typename> class UniquePtr;

	class GuiLayout
	{
	public:
		enum class Placement
		{
			Stretch,
			Center,
		};

		GuiLayout(const GuiPrivate&, Placement);

		void set_size(const SizeF& size) { _size = size; }

		Widget& add_widget(const StaticString& type, GuiPropertyLoader&);

		void update(const RectF&);
		void render(Renderer&, const Widget* hover_widget, const Widget* click_widget) const;

	private:
		const GuiPrivate& _gui;
		const Placement _placement;
		SizeF _size;
		Vector2 _scaling{1, 1};
		StdVector<UniquePtr<Widget>> _widgets;
	};
}

#endif
