#ifndef _src_gui_ion_loader_h_
#define _src_gui_ion_loader_h_

#include <yttrium/flags.h>
#include "classes.h"

namespace Yttrium
{
	class GuiLayout;
	class GuiPrivate;
	class GuiScreen;
	class IonNode;
	class StaticString;

	class GuiIonLoader
	{
	public:
		GuiIonLoader(GuiPrivate&);

		void load(const StaticString& source_name);

	private:
		enum class Attribute;

		static Attribute load_attribute(const StaticString&);

		void load(const IonObject&);

		void load_class(const IonNode&, Flags<Attribute>);
		void load_cursor(const IonNode&, Flags<Attribute>);
		void load_font(const IonNode&, Flags<Attribute>);
		void load_include(const IonNode&, Flags<Attribute>);
		void load_screen(const IonNode&, Flags<Attribute>);
		void load_on_key(const IonNode&, Flags<Attribute>);
		void load_translation(const IonNode&, Flags<Attribute>);

		void load_screen_cursor(GuiScreen&, const IonNode&, int extra) const;
		void load_screen_layout(GuiScreen&, const IonNode&, int extra) const;
		void load_screen_music(GuiScreen&, const IonNode&, int extra) const;
		void load_screen_on_enter(GuiScreen&, const IonNode&, int extra) const;
		void load_screen_on_event(GuiScreen&, const IonNode&, int extra) const;
		void load_screen_on_key(GuiScreen&, const IonNode&, int extra) const;
		void load_screen_on_return(GuiScreen&, const IonNode&, int extra) const;

		void load_layout_size(GuiLayout&, const IonNode&) const;

	private:
		GuiPrivate& _gui;
		GuiClasses _classes;
		bool _has_default_font = false;
		std::string _default_font_name;
	};
}

#endif
