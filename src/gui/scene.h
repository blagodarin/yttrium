#ifndef _src_gui_scene_h_
#define _src_gui_scene_h_

#include <yttrium/bindings.h>
#include <yttrium/rect.h>
#include <yttrium/std/map.h>
#include <yttrium/std/vector.h>
#include "types.h"

namespace Yttrium
{
	class GuiImpl;
	class GuiIonDumper;
	class GuiPropertyLoader;
	class Renderer;
	class Widget;
	template <typename> class Pointer;

	class GuiScene
	{
		friend GuiIonDumper;

	public:

		GuiScene(GuiImpl& gui, const StaticString& name, bool is_transparent);
		~GuiScene();

	public:

		void bind(const StaticString& name, const StaticString& action)
		{
			_bindings.bind(name, action);
		}

		bool is_transparent() const
		{
			return _is_transparent;
		}

		void load_widget(const StaticString& type, const StaticString& name, GuiPropertyLoader& loader);

		const String& name() const
		{
			return _name;
		}

		bool process_key(const KeyEvent& event);

		void render(Renderer& renderer); // TODO: Make const.

		void reserve(size_t capacity);

		void set_cursor(const Vector2& cursor)
		{
			_is_cursor_set = true;
			_cursor = cursor;
		}

		void set_scaling(Scaling scaling)
		{
			_scaling = scaling;
		}

		void set_size(const Vector2& size)
		{
			_size = size;
			_has_size = true;
		}

	private:

		RectF map(const RectF& source, const Vector2& shift, const Vector2& scale, Scaling scaling) const;

	private:

		GuiImpl&                   _gui;
		String                     _name;
		Vector2                    _size;
		bool                       _has_size = false;
		Scaling                    _scaling = Scaling::Stretch;
		StdVector<Pointer<Widget>> _widgets;
		StdMap<String, Widget*>    _named_widgets;
		bool                       _is_cursor_set = false;
		Vector2                    _cursor;
		Widget*                    _mouse_widget = nullptr;
		const Widget*              _left_click_widget = nullptr;
		Widget*                    _focus_widget = nullptr;
		const bool                 _is_transparent;
		Bindings                   _bindings;
	};
}

#endif
