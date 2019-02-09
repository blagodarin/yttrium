//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <yttrium/utils/flags.h>
#include "../widget_data.h"

#include <unordered_map>

namespace Yttrium
{
	class GuiActions;
	class GuiLayout;
	class GuiPrivate;
	class GuiScreen;
	class IonReader;
	class IonToken;
	class MusicReader;
	class ResourceLoader;
	class Translation;
	class Widget;

	class GuiIonLoader
	{
	public:
		GuiIonLoader(GuiPrivate&, ResourceLoader&);
		~GuiIonLoader() noexcept;

		void load(std::string_view source_name);

		static bool read_texture_filter(IonReader&, IonToken&, Texture2D::Filter&);

	private:
		enum class Attribute;

		static Attribute load_attribute(std::string_view);

		void load(IonReader&);

		void load_class(IonReader&, IonToken&, Flags<Attribute>);
		void load_cursor(IonReader&, IonToken&, Flags<Attribute>);
		void load_font(IonReader&, IonToken&, Flags<Attribute>);
		void load_icon(IonReader&, IonToken&, Flags<Attribute>);
		void load_include(IonReader&, IonToken&, Flags<Attribute>);
		void load_music(IonReader&, IonToken&, Flags<Attribute>);
		void load_on_key(IonReader&, IonToken&, Flags<Attribute>);
		void load_screen(IonReader&, IonToken&, Flags<Attribute>);
		void load_script(IonReader&, IonToken&, Flags<Attribute>);
		void load_title(IonReader&, IonToken&, Flags<Attribute>);
		void load_translation(IonReader&, IonToken&, Flags<Attribute>);

		void load_screen_cursor(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_layout(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_music(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_on_enter(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_on_event(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_on_key(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_on_return(GuiScreen&, IonReader&, IonToken&, int extra) const;
		void load_screen_transparent(GuiScreen&, IonReader&, IonToken&, int extra) const;

		std::unique_ptr<WidgetData> load_widget(IonReader&, IonToken&) const;
		void load_widget_on_click(WidgetData&, IonReader&, IonToken&) const;
		void load_widget_on_enter(WidgetData&, IonReader&, IonToken&) const;
		void load_widget_on_update(WidgetData&, IonReader&, IonToken&) const;
		void load_widget_position(WidgetData&, IonReader&, IonToken&) const;
		void load_widget_sound(WidgetData&, IonReader&, IonToken&) const;
		void load_widget_state(WidgetData&, IonReader&, IonToken&) const;
		void load_widget_text(WidgetData&, IonReader&, IonToken&) const;

		void load_style_align(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_borders(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_color(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_font(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_text_color(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_text_size(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_texture(WidgetData::StyleData&, IonReader&, IonToken&) const;
		void load_style_texture_rect(WidgetData::StyleData&, IonReader&, IonToken&) const;

	private:
		GuiPrivate& _gui;
		ResourceLoader& _resource_loader;
		std::shared_ptr<const Font> _default_font;
		std::unordered_map<std::string, std::unique_ptr<WidgetData>> _prototypes;
		std::unordered_map<std::string, std::shared_ptr<MusicReader>> _music;
		std::unordered_map<std::string, std::shared_ptr<const Font>> _fonts;
		std::shared_ptr<const Translation> _translation;
	};
}
