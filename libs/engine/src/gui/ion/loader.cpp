//
// This file is part of the Yttrium toolkit
// Copyright (C) 2019 Sergei Blagodarin
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

#include "loader.h"

#include <yttrium/audio/reader.h>
#include <yttrium/exceptions.h>
#include <yttrium/ion/reader.h>
#include <yttrium/renderer/textured_rect.h>
#include <yttrium/resource_loader.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
#include <yttrium/translation.h>
#include "../../../../core/src/utils/string.h"
#include "../gui.h"
#include "../screen.h"
#include "../widgets/button.h"
#include "../widgets/canvas.h"
#include "../widgets/image.h"
#include "../widgets/input.h"
#include "../widgets/label.h"

namespace
{
	using namespace Yttrium;

	struct OnKey
	{
		std::string_view _key;
		GuiActions _on_press;
		GuiActions _on_release;
	};

	GuiActions read_actions(IonReader& ion, IonToken& token)
	{
		GuiActions actions;
		token.check_object_begin();
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd; token.next(ion))
		{
			const auto action = token.to_name();
			if (action == "call")
				actions.add<GuiAction_Call>(token.next(ion).to_value());
			else if (action == "enter")
				actions.add<GuiAction_Enter>(token.next(ion).to_value());
			else if (action == "quit")
				actions.add<GuiAction_Quit>();
			else if (action == "return")
				actions.add<GuiAction_Return>();
			else if (action == "return_to")
				actions.add<GuiAction_ReturnTo>(token.next(ion).to_value());
			else
				throw GuiDataError{ "Unknown action \"", action, "\"" };
		}
		token.next(ion);
		return actions;
	}

	OnKey read_on_key(IonReader& ion, IonToken& token)
	{
		OnKey on_key;
		on_key._key = token.to_value();
		on_key._on_press = ::read_actions(ion, token.next(ion));
		if (token.type() == IonToken::Type::ObjectBegin)
			on_key._on_release = ::read_actions(ion, token);
		return on_key;
	}

	bool update_rect(RectF& rect, IonReader& ion, IonToken& token)
	{
		if (token.type() != IonToken::Type::StringValue)
			return false;
		if (!token.text().empty())
		{
			float x = 0;
			if (!from_chars(token.text(), x))
				return false;
			rect = { { x, rect.top() }, rect.size() };
		}
		if (token.next(ion).type() != IonToken::Type::StringValue)
			return true;
		if (!token.text().empty())
		{
			float y = 0;
			if (!from_chars(token.text(), y))
				return false;
			rect = { { rect.left(), y }, rect.size() };
		}
		if (token.next(ion).type() != IonToken::Type::StringValue)
			return true;
		if (!token.text().empty())
		{
			float width = 0;
			if (!from_chars(token.text(), width))
				return false;
			rect = { rect.top_left(), SizeF{ width, rect.height() } };
		}
		if (token.next(ion).type() != IonToken::Type::StringValue)
			return true;
		if (!token.text().empty())
		{
			float height = 0;
			if (!from_chars(token.text(), height))
				return false;
			rect = { rect.top_left(), SizeF{ rect.width(), height } };
		}
		token.next(ion);
		return true;
	}
}

namespace Yttrium
{
	class GuiIonLoader
	{
	public:
		GuiIonLoader(GuiPrivate&, ResourceLoader&);
		~GuiIonLoader() noexcept;

		void load(std::string_view source_name);

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
		std::unordered_map<std::string, std::shared_ptr<AudioReader>> _music;
		std::unordered_map<std::string, std::shared_ptr<const Font>> _fonts;
		std::shared_ptr<const Translation> _translation;
	};

	GuiIonLoader::GuiIonLoader(GuiPrivate& gui, ResourceLoader& resource_loader)
		: _gui{ gui }
		, _resource_loader{ resource_loader }
	{
	}

	GuiIonLoader::~GuiIonLoader() noexcept = default;

	void GuiIonLoader::load(std::string_view source_name)
	{
		const auto source = _resource_loader.open(source_name);
		IonReader ion{ *source };
		try
		{
			load(ion);
		}
		catch (GuiDataError& e)
		{
			throw GuiDataError{ source_name, ": ", e.what() };
		}
	}

	enum class GuiIonLoader::Attribute
	{
		Unknown = 0,
		Root = 1 << 0,
	};

	GuiIonLoader::Attribute GuiIonLoader::load_attribute(std::string_view name)
	{
		static const std::unordered_map<std::string_view, Attribute> attributes{
			{ "root", Attribute::Root },
		};

		const auto i = attributes.find(name);
		return i != attributes.end() ? i->second : Attribute::Unknown;
	}

	void GuiIonLoader::load(IonReader& ion)
	{
		static const std::unordered_map<std::string_view, void (GuiIonLoader::*)(IonReader&, IonToken&, Flags<Attribute>)> handlers{
			{ "class", &GuiIonLoader::load_class },
			{ "cursor", &GuiIonLoader::load_cursor },
			{ "font", &GuiIonLoader::load_font },
			{ "icon", &GuiIonLoader::load_icon },
			{ "include", &GuiIonLoader::load_include },
			{ "music", &GuiIonLoader::load_music },
			{ "on_key", &GuiIonLoader::load_on_key },
			{ "screen", &GuiIonLoader::load_screen },
			{ "script", &GuiIonLoader::load_script },
			{ "title", &GuiIonLoader::load_title },
			{ "translation", &GuiIonLoader::load_translation },
		};

		for (auto token = ion.read(); token.type() != Yttrium::IonToken::Type::End;)
		{
			Flags<Attribute> attributes;
			for (;;)
			{
				const auto attribute = load_attribute(token.to_name());
				if (attribute == Attribute::Unknown)
					break;
				attributes |= attribute;
				token.next(ion);
			}
			const auto i = handlers.find(token.text());
			if (i == handlers.end())
				throw GuiDataError{ "Unknown entry '", token.text(), "'" };
			(this->*i->second)(ion, token.next(ion), attributes);
		}
	}

	void GuiIonLoader::load_class(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		std::string name{ token.to_value() };
		const auto i = _prototypes.find(name);
		if (i != _prototypes.end())
			throw GuiDataError{ "Duplicate 'class' \"", name, "\"" };
		_prototypes.emplace(std::move(name), load_widget(ion, token.next(ion)));
	}

	void GuiIonLoader::load_cursor(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		token.check_object_begin();
		const auto type = token.next(ion).to_name();
		if (type == "none")
			_gui.set_default_cursor(GuiCursor::None, {});
		else if (type == "custom")
			_gui.set_default_cursor(GuiCursor::Custom, {});
		else if (type == "texture")
			_gui.set_default_cursor(GuiCursor::Texture, _resource_loader.load_texture_2d(token.next(ion).to_value()));
		else if (type != "default")
			throw GuiDataError{ "Unknown cursor type '", type, "'" };
		token.next(ion).check_object_end();
		token.next(ion);
	}

	void GuiIonLoader::load_font(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		// TODO: Use 'font <path>' for setting the default font (until the next 'font' command).
		const auto font_name = token.to_value();
		token.next(ion).check_object_begin();
		std::shared_ptr<const Font> font;
		bool set_as_default = false;
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd; token.next(ion))
		{
			const auto name = token.to_name();
			if (name == "default")
				set_as_default = true;
			else if (name == "file")
				font = Font::load(*_resource_loader.open(token.next(ion).to_value()), *_resource_loader.render_manager()); // TODO: RenderManager* may be nullptr!
			else
				throw GuiDataError{ "Unknown font option '", name, "'" };
		}
		token.next(ion);
		if (set_as_default)
			_default_font = font;
		_fonts.insert_or_assign(std::string{ font_name }, std::move(font));
	}

	void GuiIonLoader::load_icon(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		_gui.set_icon_path(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_include(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		load(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_music(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		std::string_view music_name = token.to_value();
		if (music_name.empty())
			throw GuiDataError{ "Empty music name" };
		std::string_view music_file;
		int music_end = 0;
		int music_loop = 0;
		token.next(ion).check_object_begin();
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd;)
		{
			if (const auto entry_name = token.to_name(); entry_name == "file")
				music_file = token.next(ion).to_value();
			else if (entry_name == "end")
				music_end = time_from_chars(token.next(ion).to_value());
			else if (entry_name == "loop")
				music_loop = time_from_chars(token.next(ion).to_value());
			else
				throw GuiDataError{ "Unknown music option '", token.text(), "'" };
			token.next(ion);
		}
		if (music_file.empty())
			throw GuiDataError{ "No music file specified" };
		token.next(ion);
		//
		auto music = std::make_shared<AudioReader>(_resource_loader.open(music_file));
		music->set_loop(std::chrono::milliseconds{ music_loop }, std::chrono::milliseconds{ music_end });
		_music.insert_or_assign(std::string{ music_name }, std::move(music));
	}

	void GuiIonLoader::load_on_key(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		auto on_key = ::read_on_key(ion, token);
		_gui.set_on_key(on_key._key, std::move(on_key._on_press), std::move(on_key._on_release));
	}

	void GuiIonLoader::load_screen(IonReader& ion, IonToken& token, Flags<Attribute> attributes)
	{
		static const std::unordered_map<std::string_view, std::pair<void (GuiIonLoader::*)(GuiScreen&, IonReader&, IonToken&, int) const, int>> handlers{
			{ "center", { &GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Center) } },
			{ "cursor", { &GuiIonLoader::load_screen_cursor, 0 } },
			{ "left", { &GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Left) } },
			{ "music", { &GuiIonLoader::load_screen_music, 0 } },
			{ "on_enter", { &GuiIonLoader::load_screen_on_enter, 0 } },
			{ "on_event", { &GuiIonLoader::load_screen_on_event, 0 } },
			{ "on_key", { &GuiIonLoader::load_screen_on_key, 0 } },
			{ "on_return", { &GuiIonLoader::load_screen_on_return, 0 } },
			{ "right", { &GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Right) } },
			{ "stretch", { &GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Stretch) } },
			{ "transparent", { &GuiIonLoader::load_screen_transparent, 0 } },
		};

		auto& screen = _gui.add_screen(token.to_value(), attributes & Attribute::Root);
		token.next(ion).check_object_begin();
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd;)
		{
			const auto i = handlers.find(token.to_name());
			if (i == handlers.end())
				throw GuiDataError{ "Unknown screen entry '", token.text(), "'" };
			(this->*i->second.first)(screen, ion, token.next(ion), i->second.second);
		}
		token.next(ion);
	}

	void GuiIonLoader::load_script(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		token.check_object_begin();
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd;)
		{
			const auto name = token.to_name();
			std::vector<std::string> args;
			for (token.next(ion); token.type() == IonToken::Type::StringValue; token.next(ion))
				args.emplace_back(token.translatable() && _translation ? _translation->translate(token.text()) : token.text());
			_gui.add_startup_command(name, std::move(args));
		}
		token.next(ion);
	}

	void GuiIonLoader::load_title(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		_gui.set_title(token.translatable() && _translation ? _translation->translate(token.to_value()) : token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_translation(IonReader& ion, IonToken& token, Flags<Attribute>)
	{
		_translation = _resource_loader.load_translation(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_screen_cursor(GuiScreen& screen, IonReader& ion, IonToken& token, int) const
	{
		token.check_object_begin();
		const auto type = token.next(ion).to_name();
		if (type == "none")
			screen.set_cursor(GuiCursor::None, {});
		else if (type == "custom")
			screen.set_cursor(GuiCursor::Custom, {});
		else if (type == "texture")
			screen.set_cursor(GuiCursor::Texture, _resource_loader.load_texture_2d(token.next(ion).to_value()));
		else if (type != "default")
			throw GuiDataError{ "Unknown cursor type '", type, "'" };
		token.next(ion).check_object_end();
		token.next(ion);
	}

	void GuiIonLoader::load_screen_layout(GuiScreen& screen, IonReader& ion, IonToken& token, int extra) const
	{
		static const std::unordered_map<std::string_view, std::unique_ptr<Widget> (*)(GuiPrivate&, std::unique_ptr<WidgetData>&&, std::string_view)> handlers{
			{ "button",
				[](GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view) -> std::unique_ptr<Widget> {
					return std::make_unique<ButtonWidget>(gui, std::move(data));
				} },
			{ "canvas",
				[](GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view name) -> std::unique_ptr<Widget> {
					return std::make_unique<CanvasWidget>(gui, std::move(data), name);
				} },
			{ "image",
				[](GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view) -> std::unique_ptr<Widget> {
					return std::make_unique<ImageWidget>(gui, std::move(data));
				} },
			{ "input",
				[](GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view) -> std::unique_ptr<Widget> {
					return std::make_unique<InputWidget>(gui, std::move(data));
				} },
			{ "label",
				[](GuiPrivate& gui, std::unique_ptr<WidgetData>&& data, std::string_view) -> std::unique_ptr<Widget> {
					return std::make_unique<LabelWidget>(gui, std::move(data));
				} },
		};

		const auto placement = static_cast<GuiLayout::Placement>(extra);
		auto& layout = screen.add_layout(placement);
		SizeF size{ 0, 0 };
		if (placement == GuiLayout::Placement::Center)
		{
			if (!from_chars(token.to_value(), size._width) || !from_chars(token.next(ion).to_value(), size._height))
				throw GuiDataError{ "Bad layout size" };
			token.next(ion);
		}
		else if (placement == GuiLayout::Placement::Left || placement == GuiLayout::Placement::Right)
		{
			if (!from_chars(token.to_value(), size._height))
				throw GuiDataError{ "Bad layout size" };
			token.next(ion);
		}
		else if (token.type() == IonToken::Type::StringValue)
		{
			if (!from_chars(token.text(), size._width) || !from_chars(token.next(ion).to_value(), size._height))
				throw GuiDataError{ "Bad layout size" };
			token.next(ion);
		}
		layout.set_size(size);
		token.check_object_begin();
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd;)
		{
			const auto i = handlers.find(token.to_name());
			if (i == handlers.end())
				throw GuiDataError{ "Unknown widget '", token.text(), "'" };
			std::string_view name;
			if (token.next(ion).type() == IonToken::Type::StringValue)
			{
				name = token.text();
				token.next(ion);
			}
			screen.register_widget(layout.add_widget(i->second(_gui, load_widget(ion, token), name)));
		}
		token.next(ion);
	}

	void GuiIonLoader::load_screen_music(GuiScreen& screen, IonReader& ion, IonToken& token, int) const
	{
		std::shared_ptr<AudioReader> music;
		if (const std::string music_name{ token.to_value() }; !music_name.empty())
		{
			const auto i = _music.find(music_name);
			if (i == _music.end())
				throw GuiDataError{ "Unknown music \"", music_name, "\"" };
			music = i->second;
		}
		token.next(ion);
		screen.set_music(std::move(music));
	}

	void GuiIonLoader::load_screen_on_enter(GuiScreen& screen, IonReader& ion, IonToken& token, int) const
	{
		screen.set_on_enter(::read_actions(ion, token));
	}

	void GuiIonLoader::load_screen_on_event(GuiScreen& screen, IonReader& ion, IonToken& token, int) const
	{
		const auto event_name = token.to_value();
		screen.set_on_event(event_name, ::read_actions(ion, token.next(ion)));
	}

	void GuiIonLoader::load_screen_on_key(GuiScreen& screen, IonReader& ion, IonToken& token, int) const
	{
		auto on_key = ::read_on_key(ion, token);
		screen.set_on_key(on_key._key, std::move(on_key._on_press), std::move(on_key._on_release));
	}

	void GuiIonLoader::load_screen_on_return(GuiScreen& screen, IonReader& ion, IonToken& token, int) const
	{
		screen.set_on_return(::read_actions(ion, token));
	}

	void GuiIonLoader::load_screen_transparent(GuiScreen& screen, IonReader&, IonToken&, int) const
	{
		screen.set_transparent(true);
	}

	std::unique_ptr<WidgetData> GuiIonLoader::load_widget(IonReader& ion, IonToken& token) const
	{
		static const std::unordered_map<std::string_view, void (GuiIonLoader::*)(WidgetData&, IonReader&, IonToken&) const> handlers{
			{ "on_click", &GuiIonLoader::load_widget_on_click },
			{ "on_enter", &GuiIonLoader::load_widget_on_enter },
			{ "on_update", &GuiIonLoader::load_widget_on_update },
			{ "position", &GuiIonLoader::load_widget_position },
			{ "sound", &GuiIonLoader::load_widget_sound },
			{ "state", &GuiIonLoader::load_widget_state },
			{ "text", &GuiIonLoader::load_widget_text },
		};

		static const std::unordered_map<std::string_view, WidgetData::Style> styles{
			{ "checked", WidgetData::Style::Checked },
			{ "disabled", WidgetData::Style::Disabled },
			{ "hovered", WidgetData::Style::Hovered },
			{ "pressed", WidgetData::Style::Pressed },
		};

		const auto load_style_entry = [this, &ion, &token](WidgetData::StyleData& data) {
			static const std::unordered_map<std::string_view, void (GuiIonLoader::*)(WidgetData::StyleData&, IonReader&, IonToken&) const> style_handlers{
				{ "align", &GuiIonLoader::load_style_align },
				{ "borders", &GuiIonLoader::load_style_borders },
				{ "color", &GuiIonLoader::load_style_color },
				{ "font", &GuiIonLoader::load_style_font },
				{ "text_color", &GuiIonLoader::load_style_text_color },
				{ "text_size", &GuiIonLoader::load_style_text_size },
				{ "texture", &GuiIonLoader::load_style_texture },
				{ "texture_rect", &GuiIonLoader::load_style_texture_rect },
			};
			const auto i = style_handlers.find(token.to_name());
			if (i == style_handlers.end())
				return false;
			(this->*i->second)(data, ion, token.next(ion));
			return true;
		};

		std::unique_ptr<WidgetData> data;
		if (token.type() == IonToken::Type::ListBegin)
		{
			const auto i = _prototypes.find(std::string{ token.next(ion).to_value() });
			if (i == _prototypes.end())
				throw GuiDataError{ "Unknown class \"", token.text(), "\"" };
			data = std::make_unique<WidgetData>(*i->second);
			token.next(ion).check_list_end();
			token.next(ion);
		}
		else
		{
			data = std::make_unique<WidgetData>();
			data->_styles[WidgetData::Style::Normal]._foreground._font = _default_font;
		}
		token.check_object_begin();
		for (token.next(ion); token.type() != IonToken::Type::ObjectEnd;)
		{
			const auto i = handlers.find(token.to_name());
			if (i != handlers.end())
			{
				(this->*i->second)(*data, ion, token.next(ion));
				continue;
			}
			const auto j = styles.find(token.text());
			if (j != styles.end())
			{
				auto k = data->_styles.find(j->second);
				if (k == data->_styles.end())
					k = data->_styles.emplace(j->second, data->_styles[WidgetData::Style::Normal]).first;
				token.next(ion).check_object_begin();
				for (token.next(ion); token.type() != IonToken::Type::ObjectEnd;)
					if (!load_style_entry(k->second))
						throw GuiDataError{ "Unknown '", j->first, "' entry '", token.text(), "'" };
				token.next(ion);
			}
			else if (!load_style_entry(data->_styles[WidgetData::Style::Normal]))
				throw GuiDataError{ "Unknown widget entry '", token.text(), "'" };
		}
		token.next(ion);
		return data;
	}

	void GuiIonLoader::load_widget_on_click(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		data._actions.insert_or_assign(WidgetData::Action::OnClick, ::read_actions(ion, token));
	}

	void GuiIonLoader::load_widget_on_enter(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		data._actions.insert_or_assign(WidgetData::Action::OnEnter, ::read_actions(ion, token));
	}

	void GuiIonLoader::load_widget_on_update(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		data._actions.insert_or_assign(WidgetData::Action::OnUpdate, ::read_actions(ion, token));
	}

	void GuiIonLoader::load_widget_position(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		if (!::update_rect(data._rect, ion, token))
			throw GuiDataError{ "Bad 'position'" };
	}

	void GuiIonLoader::load_widget_sound(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		data._sound = _gui.load_sound(_resource_loader, token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_widget_state(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		static const std::unordered_map<std::string_view, WidgetData::Style> states{
			{ "checked", WidgetData::Style::Checked },
			{ "disabled", WidgetData::Style::Disabled },
			{ "hovered", WidgetData::Style::Hovered },
			{ "normal", WidgetData::Style::Normal },
			{ "pressed", WidgetData::Style::Pressed },
		};

		const auto i = states.find(token.to_value());
		if (i == states.end())
			throw GuiDataError{ "Unknown 'state' \"", token.text(), "\"" };
		data._fixed_style = i->second;
		token.next(ion);
	}

	void GuiIonLoader::load_widget_text(WidgetData& data, IonReader& ion, IonToken& token) const
	{
		data._text = token.translatable() && _translation ? _translation->translate(token.to_value()) : token.to_value();
		token.next(ion);
	}

	void GuiIonLoader::load_style_align(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		static const std::unordered_map<std::string_view, unsigned> alignment_flags{
			{ "bottom", BottomAlignment },
			{ "center", CenterAlignment },
			{ "left", LeftAlignment },
			{ "right", RightAlignment },
			{ "top", TopAlignment },
		};

		const auto i = alignment_flags.find(token.to_value());
		if (i == alignment_flags.end())
			throw GuiDataError{ "Bad 'align' \"", token.text(), "\"" };
		auto alignment = i->second;
		if (token.next(ion).type() == IonToken::Type::StringValue)
		{
			if (alignment != TopAlignment && alignment != BottomAlignment)
				throw GuiDataError{ "Bad 'align' \"", i->first, "\" \"", token.text(), "\"" };
			const auto j = alignment_flags.find(token.text());
			if (j == alignment_flags.end())
				throw GuiDataError{ "Bad 'align' \"", i->first, "\" \"", token.text(), "\"" };
			if (j->second != LeftAlignment && j->second != RightAlignment)
				throw GuiDataError{ "Bad 'align' \"", i->first, "\" \"", token.text(), "\"" };
			alignment |= j->second;
			token.next(ion);
		}
		data._foreground._alignment = alignment;
	}

	void GuiIonLoader::load_style_borders(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		auto& borders = data._background.borders;
		if (!from_chars(token.to_value(), borders._top)
			|| !from_chars(token.next(ion).to_value(), borders._right)
			|| !from_chars(token.next(ion).to_value(), borders._bottom)
			|| !from_chars(token.next(ion).to_value(), borders._left))
			throw GuiDataError{ "Bad 'borders'" };
		token.next(ion);
	}

	void GuiIonLoader::load_style_color(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		data._background.color = token.to_color();
		token.next(ion);
	}

	void GuiIonLoader::load_style_font(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		const std::string font_name{ token.to_value() };
		token.next(ion);
		//
		const auto i = _fonts.find(font_name);
		if (i == _fonts.end())
			throw GuiDataError{ "Unknown font \"", font_name, "\"" };
		data._foreground._font = i->second;
	}

	void GuiIonLoader::load_style_text_color(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		data._foreground._color = token.to_color();
		token.next(ion);
	}

	void GuiIonLoader::load_style_text_size(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		if (!from_chars(token.to_value(), data._foreground._size))
			throw GuiDataError{ "Bad 'text_size'" };
		token.next(ion);
	}

	void GuiIonLoader::load_style_texture(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		auto& background = data._background;
		background.texture = _resource_loader.load_texture_2d(token.to_value());
		if (!read_ion_texture_filter(ion, token.next(ion), background.texture_filter))
			throw GuiDataError{ "Bad 'texture' filter '", token.text(), "'" };
		background.texture_rect = RectF{ Rect{ background.texture->size() } };
	}

	void GuiIonLoader::load_style_texture_rect(WidgetData::StyleData& data, IonReader& ion, IonToken& token) const
	{
		if (!::update_rect(data._background.texture_rect, ion, token))
			throw GuiDataError{ "Bad 'texture_rect'" };
	}

	void load_ion_gui(GuiPrivate& gui, ResourceLoader& resource_loader, std::string_view path)
	{
		GuiIonLoader{ gui, resource_loader }.load(path);
	}

	bool read_ion_texture_filter(IonReader& ion, IonToken& token, Texture2D::Filter& filter)
	{
		static const std::unordered_map<std::string_view, Texture2D::Filter> filters{
			{ "nearest", Texture2D::NearestFilter },
			{ "linear", Texture2D::LinearFilter },
			{ "bilinear", Texture2D::BilinearFilter },
			{ "trilinear", Texture2D::TrilinearFilter },
		};

		if (token.type() != IonToken::Type::ObjectBegin)
			return true;
		if (const auto i = filters.find(token.next(ion).to_name()); i != filters.end())
			filter = i->second;
		else
			return false;
		if (token.next(ion).type() == IonToken::Type::Name && token.text() == "anisotropic")
		{
			filter = static_cast<Texture2D::Filter>(filter | Texture2D::AnisotropicFilter);
			token.next(ion);
		}
		token.check_object_end();
		token.next(ion);
		return true;
	}
}
