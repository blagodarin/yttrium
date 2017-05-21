#ifndef _src_gui_ion_loader_h_
#define _src_gui_ion_loader_h_

#include <yttrium/flags.h>
#include <yttrium/ion/reader.h>
#include <yttrium/std/string_view.h>
#include "../widget_data.h"

#include <functional>
#include <unordered_map>

namespace Yttrium
{
	class GuiActions;
	class GuiLayout;
	class GuiPrivate;
	class GuiScreen;
	class Widget;

	class GuiIonLoader
	{
	public:
		explicit GuiIonLoader(GuiPrivate&);

		void load(std::string_view source_name);

		static bool read_texture_filter(IonReader&, IonReader::Token&, Texture2D::Filter&);

	private:
		enum class Attribute;

		static Attribute load_attribute(std::string_view);

		void load(IonReader&);

		void load_class(IonReader&, IonReader::Token&, Flags<Attribute>);
		void load_cursor(IonReader&, IonReader::Token&, Flags<Attribute>);
		void load_font(IonReader&, IonReader::Token&, Flags<Attribute>);
		void load_include(IonReader&, IonReader::Token&, Flags<Attribute>);
		void load_screen(IonReader&, IonReader::Token&, Flags<Attribute>);
		void load_on_key(IonReader&, IonReader::Token&, Flags<Attribute>);
		void load_translation(IonReader&, IonReader::Token&, Flags<Attribute>);

		void load_screen_cursor(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;
		void load_screen_layout(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;
		void load_screen_music(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;
		void load_screen_on_enter(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;
		void load_screen_on_event(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;
		void load_screen_on_key(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;
		void load_screen_on_return(GuiScreen&, IonReader&, IonReader::Token&, int extra) const;

		std::unique_ptr<WidgetData> load_widget(IonReader&, IonReader::Token&) const;
		void load_widget_on_click(WidgetData&, IonReader&, IonReader::Token&) const;
		void load_widget_on_enter(WidgetData&, IonReader&, IonReader::Token&) const;
		void load_widget_on_update(WidgetData&, IonReader&, IonReader::Token&) const;
		void load_widget_position(WidgetData&, IonReader&, IonReader::Token&) const;
		void load_widget_sound(WidgetData&, IonReader&, IonReader::Token&) const;
		void load_widget_state(WidgetData&, IonReader&, IonReader::Token&) const;
		void load_widget_text(WidgetData&, IonReader&, IonReader::Token&) const;

		void load_style_align(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_borders(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_color(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_font(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_text_color(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_text_size(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_texture(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;
		void load_style_texture_rect(WidgetData::StyleData&, IonReader&, IonReader::Token&) const;

	private:
		GuiPrivate& _gui;
		std::unordered_map<std::string_view, std::function<std::unique_ptr<Widget>(std::string_view, std::unique_ptr<WidgetData>&&)>> _widget_factory;
		std::shared_ptr<const TextureFont> _default_font;
		std::shared_ptr<const Texture2D> _default_font_texture;
		std::unordered_map<std::string, std::unique_ptr<WidgetData>> _prototypes;
	};
}

#endif
