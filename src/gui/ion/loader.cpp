#include "loader.h"

#include <yttrium/exceptions.h>
#include <yttrium/renderer/textured_rect.h>
#include <yttrium/resource_loader.h>
#include <yttrium/std/optional.h>
#include <yttrium/storage/source.h>
#include <yttrium/storage/storage.h>
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

	GuiActions read_actions(IonReader& ion, IonReader::Token& token)
	{
		GuiActions actions;
		token.check_object_begin();
		for (token.next(ion); token.type() != IonReader::Token::Type::ObjectEnd; token.next(ion))
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
				throw GuiDataError{"Unknown action \"", action, "\""};
		}
		token.next(ion);
		return actions;
	}

	OnKey read_on_key(IonReader& ion, IonReader::Token& token)
	{
		OnKey on_key;
		on_key._key = token.to_value();
		on_key._on_press = ::read_actions(ion, token.next(ion));
		if (token.type() == IonReader::Token::Type::ObjectBegin)
			on_key._on_release = ::read_actions(ion, token);
		return on_key;
	}

	bool update_color(Color4f& color, IonReader& ion, IonReader::Token& token)
	{
		if (!strings::to_number(token.to_value(), color.r)
			|| !strings::to_number(token.next(ion).to_value(), color.g)
			|| !strings::to_number(token.next(ion).to_value(), color.b))
			return false;
		if (token.next(ion).type() == IonReader::Token::Type::Value)
		{
			if (!strings::to_number(token.text(), color.a))
				return false;
			token.next(ion);
		}
		else
			color.a = 1;
		return true;
	}

	bool update_rect(RectF& rect, IonReader& ion, IonReader::Token& token)
	{
		if (token.type() != IonReader::Token::Type::Value)
			return true;
		if (!token.text().empty())
		{
			float x = 0;
			if (!strings::to_number(token.text(), x))
				return false;
			rect = {{x, rect.top()}, rect.size()};
		}
		if (token.next(ion).type() != IonReader::Token::Type::Value)
			return true;
		if (!token.text().empty())
		{
			float y = 0;
			if (!strings::to_number(token.text(), y))
				return false;
			rect = {{rect.left(), y}, rect.size()};
		}
		if (token.next(ion).type() != IonReader::Token::Type::Value)
			return true;
		if (!token.text().empty())
		{
			float width = 0;
			if (!strings::to_number(token.text(), width))
				return false;
			rect = {rect.top_left(), SizeF{width, rect.height()}};
		}
		if (token.next(ion).type() != IonReader::Token::Type::Value)
			return true;
		if (!token.text().empty())
		{
			float height = 0;
			if (!strings::to_number(token.text(), height))
				return false;
			rect = {rect.top_left(), SizeF{rect.width(), height}};
		}
		token.next(ion);
		return true;
	}
}

namespace Yttrium
{
	GuiIonLoader::GuiIonLoader(GuiPrivate& gui)
		: _gui{gui}
	{
		_widget_factory.emplace("button", [this](auto name, auto&& data){ return std::make_unique<ButtonWidget>(_gui, name, std::move(data)); });
		_widget_factory.emplace("canvas", [this](auto name, auto&& data){ return std::make_unique<CanvasWidget>(_gui, name, std::move(data)); });
		_widget_factory.emplace("image", [this](auto name, auto&& data){ return std::make_unique<ImageWidget>(_gui, name, std::move(data)); });
		_widget_factory.emplace("input", [this](auto name, auto&& data){ return std::make_unique<InputWidget>(_gui, name, std::move(data)); });
		_widget_factory.emplace("label", [this](auto name, auto&& data){ return std::make_unique<LabelWidget>(_gui, name, std::move(data)); });
	}

	void GuiIonLoader::load(std::string_view source_name)
	{
		const auto source = _gui.resource_loader().open(source_name);
		IonReader ion{*source};
		try
		{
			load(ion);
		}
		catch (GuiDataError& e)
		{
			throw GuiDataError{source_name, ": ", e.what()};
		}
	}

	bool GuiIonLoader::read_texture_filter(IonReader& ion, IonReader::Token& token, Texture2D::Filter& filter)
	{
		if (token.type() != IonReader::Token::Type::ObjectBegin)
			return true;
		const auto filter_name = token.next(ion).to_name();
		if (filter_name == "nearest")
			filter = Texture2D::NearestFilter;
		else if (filter_name == "linear")
			filter = Texture2D::LinearFilter;
		else if (filter_name == "bilinear")
			filter = Texture2D::BilinearFilter;
		else if (filter_name == "trilinear")
			filter = Texture2D::TrilinearFilter;
		else
			return false;
		if (token.next(ion).type() == IonReader::Token::Type::Name && token.text() == "anisotropic")
		{
			filter = static_cast<Texture2D::Filter>(filter | Texture2D::AnisotropicFilter);
			token.next(ion);
		}
		token.check_object_end();
		token.next(ion);
		return true;
	}

	enum class GuiIonLoader::Attribute
	{
		Unknown     = 0,
		Default     = 1 << 0,
		Root        = 1 << 1,
		Transparent = 1 << 2,
	};

	GuiIonLoader::Attribute GuiIonLoader::load_attribute(std::string_view name)
	{
		if (name == "default")
			return Attribute::Default;
		else if (name == "root")
			return Attribute::Root;
		else if (name == "transparent")
			return Attribute::Transparent;
		else
			return Attribute::Unknown;
	}

	void GuiIonLoader::load(IonReader& ion)
	{
		static const std::unordered_map<std::string_view, void (GuiIonLoader::*)(IonReader&, IonReader::Token&, Flags<Attribute>)> handlers =
		{
			{"class", &GuiIonLoader::load_class},
			{"cursor", &GuiIonLoader::load_cursor},
			{"font", &GuiIonLoader::load_font},
			{"include", &GuiIonLoader::load_include},
			{"on_key", &GuiIonLoader::load_on_key},
			{"screen", &GuiIonLoader::load_screen},
			{"translation", &GuiIonLoader::load_translation},
		};

		for (auto token = ion.read(); token.type() != Yttrium::IonReader::Token::Type::End;)
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
				throw GuiDataError{"Unknown entry '", token.text(), "'"};
			(this->*i->second)(ion, token.next(ion), attributes);
		}
	}

	void GuiIonLoader::load_class(IonReader& ion, IonReader::Token& token, Flags<Attribute>)
	{
		auto name = strings::from_view(token.to_value());
		const auto i = _prototypes.find(name);
		if (i != _prototypes.end())
			throw GuiDataError{"Duplicate 'class' \"", name, "\""};
		_prototypes.emplace(std::move(name), load_widget(ion, token.next(ion)));
	}

	void GuiIonLoader::load_cursor(IonReader& ion, IonReader::Token& token, Flags<Attribute>)
	{
		token.check_object_begin();
		const auto type = token.next(ion).to_name();
		if (type == "none")
			_gui.set_default_cursor(GuiCursor::None);
		else if (type == "custom")
			_gui.set_default_cursor(GuiCursor::Custom);
		else if (type == "texture")
			_gui.set_default_cursor(GuiCursor::Texture, token.next(ion).to_value());
		else if (type != "default")
			throw GuiDataError{"Unknown cursor type '", type, "'"};
		token.next(ion).check_object_end();
		token.next(ion);
	}

	void GuiIonLoader::load_font(IonReader& ion, IonReader::Token& token, Flags<Attribute> attributes)
	{
		std::string_view font_name = "default";
		if (token.type() == IonReader::Token::Type::Value)
		{
			font_name = token.text();
			token.next(ion);
		}
		token.check_object_begin();
		std::string_view font_path;
		std::string_view texture_path;
		for (token.next(ion); token.type() != IonReader::Token::Type::ObjectEnd; token.next(ion))
		{
			const auto name = token.to_name();
			if (name == "file")
				font_path = token.next(ion).to_value();
			else if (name == "texture")
				texture_path = token.next(ion).to_value();
			else
				throw GuiDataError{"Unknown font option '", name, "'"};
		}
		_gui.set_font(strings::from_view(font_name), font_path, texture_path);
		if (attributes & Attribute::Default)
		{
			if (_default_font)
				throw GuiDataError{"Default font redefinition"};
			const auto font_desc = _gui.font(strings::from_view(font_name));
			_default_font = font_desc->font;
			_default_font_texture = font_desc->texture;
		}
		token.next(ion);
	}

	void GuiIonLoader::load_include(IonReader& ion, IonReader::Token& token, Flags<Attribute>)
	{
		load(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_screen(IonReader& ion, IonReader::Token& token, Flags<Attribute> attributes)
	{
		static const std::unordered_map<std::string_view, std::pair<void (GuiIonLoader::*)(GuiScreen&, IonReader&, IonReader::Token&, int) const, int>> handlers =
		{
			{"center", {&GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Center)}},
			{"cursor", {&GuiIonLoader::load_screen_cursor, 0}},
			{"left", {&GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Left)}},
			{"music", {&GuiIonLoader::load_screen_music, 0}},
			{"on_enter", {&GuiIonLoader::load_screen_on_enter, 0}},
			{"on_event", {&GuiIonLoader::load_screen_on_event, 0}},
			{"on_key", {&GuiIonLoader::load_screen_on_key, 0}},
			{"on_return", {&GuiIonLoader::load_screen_on_return, 0}},
			{"right", {&GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Right)}},
			{"stretch", {&GuiIonLoader::load_screen_layout, static_cast<int>(GuiLayout::Placement::Stretch)}},
		};

		auto& screen = _gui.add_screen(strings::from_view(token.to_value()), attributes & Attribute::Transparent && !(attributes & Attribute::Root), attributes & Attribute::Root);
		token.next(ion).check_object_begin();
		for (token.next(ion); token.type() != IonReader::Token::Type::ObjectEnd;)
		{
			const auto i = handlers.find(token.to_name());
			if (i == handlers.end())
				throw GuiDataError{"Unknown screen entry '", token.text(), "'"};
			(this->*i->second.first)(screen, ion, token.next(ion), i->second.second);
		}
		token.next(ion);
	}

	void GuiIonLoader::load_on_key(IonReader& ion, IonReader::Token& token, Flags<Attribute>)
	{
		auto on_key = ::read_on_key(ion, token);
		_gui.set_on_key(on_key._key, std::move(on_key._on_press), std::move(on_key._on_release));
	}

	void GuiIonLoader::load_translation(IonReader& ion, IonReader::Token& token, Flags<Attribute>)
	{
		_gui.set_translation(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_screen_cursor(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int) const
	{
		token.check_object_begin();
		const auto type = token.next(ion).to_name();
		if (type == "none")
			screen.set_cursor(GuiCursor::None);
		else if (type == "custom")
			screen.set_cursor(GuiCursor::Custom);
		else if (type == "texture")
			screen.set_cursor(GuiCursor::Texture, token.next(ion).to_value());
		else if (type != "default")
			throw GuiDataError{"Unknown cursor type '", type, "'"};
		token.next(ion).check_object_end();
		token.next(ion);
	}

	void GuiIonLoader::load_screen_layout(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int extra) const
	{
		const auto placement = static_cast<GuiLayout::Placement>(extra);
		auto& layout = screen.add_layout(placement);
		SizeF size{0, 0};
		if (placement == GuiLayout::Placement::Center)
		{
			if (!strings::to_number(token.to_value(), size._width) || !strings::to_number(token.next(ion).to_value(), size._height))
				throw GuiDataError{"Bad layout size"};
			token.next(ion);
		}
		else if (placement == GuiLayout::Placement::Left || placement == GuiLayout::Placement::Right)
		{
			if (!strings::to_number(token.to_value(), size._height))
				throw GuiDataError{"Bad layout size"};
			token.next(ion);
		}
		else if (token.type() == IonReader::Token::Type::Value)
		{
			if (!strings::to_number(token.text(), size._width) || !strings::to_number(token.next(ion).to_value(), size._height))
				throw GuiDataError{"Bad layout size"};
			token.next(ion);
		}
		layout.set_size(size);
		token.check_object_begin();
		for (token.next(ion); token.type() != IonReader::Token::Type::ObjectEnd;)
		{
			const auto i = _widget_factory.find(token.to_name());
			if (i == _widget_factory.end())
				throw GuiDataError{"Unknown widget '", token.text(), "'"};
			std::string_view name;
			if (token.next(ion).type() == IonReader::Token::Type::Value)
			{
				name = token.text();
				token.next(ion);
			}
			screen.register_widget(layout.add_widget(i->second(name, load_widget(ion, token))));
		}
		token.next(ion);
	}

	void GuiIonLoader::load_screen_music(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int) const
	{
		const auto music_name = token.to_value();
		screen.set_music(music_name.empty() ? nullptr : _gui.resource_loader().load_music(music_name));
		token.next(ion);
	}

	void GuiIonLoader::load_screen_on_enter(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int) const
	{
		screen.set_on_enter(::read_actions(ion, token));
	}

	void GuiIonLoader::load_screen_on_event(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int) const
	{
		const auto event_name = token.to_value();
		screen.set_on_event(strings::from_view(event_name), ::read_actions(ion, token.next(ion)));
	}

	void GuiIonLoader::load_screen_on_key(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int) const
	{
		auto on_key = ::read_on_key(ion, token);
		screen.set_on_key(on_key._key, std::move(on_key._on_press), std::move(on_key._on_release));
	}

	void GuiIonLoader::load_screen_on_return(GuiScreen& screen, IonReader& ion, IonReader::Token& token, int) const
	{
		screen.set_on_return(::read_actions(ion, token));
	}

	std::unique_ptr<WidgetData> GuiIonLoader::load_widget(IonReader& ion, IonReader::Token& token) const
	{
		static const std::unordered_map<std::string_view, void (GuiIonLoader::*)(WidgetData&, IonReader&, IonReader::Token&) const> handlers =
		{
			{"on_click", &GuiIonLoader::load_widget_on_click},
			{"on_enter", &GuiIonLoader::load_widget_on_enter},
			{"on_update", &GuiIonLoader::load_widget_on_update},
			{"position", &GuiIonLoader::load_widget_position},
			{"sound", &GuiIonLoader::load_widget_sound},
			{"state", &GuiIonLoader::load_widget_state},
			{"text", &GuiIonLoader::load_widget_text},
		};

		static const std::unordered_map<std::string_view, WidgetData::Style> styles
		{
			{"checked", WidgetData::Style::Checked},
			{"disabled", WidgetData::Style::Disabled},
			{"hovered", WidgetData::Style::Hovered},
			{"pressed", WidgetData::Style::Pressed},
		};

		const auto load_style_entry = [this, &ion, &token](WidgetData::StyleData& data)
		{
			static const std::unordered_map<std::string_view, void (GuiIonLoader::*)(WidgetData::StyleData&, IonReader&, IonReader::Token&) const> style_handlers =
			{
				{"align", &GuiIonLoader::load_style_align},
				{"borders", &GuiIonLoader::load_style_borders},
				{"color", &GuiIonLoader::load_style_color},
				{"font", &GuiIonLoader::load_style_font},
				{"text_color", &GuiIonLoader::load_style_text_color},
				{"text_size", &GuiIonLoader::load_style_text_size},
				{"texture", &GuiIonLoader::load_style_texture},
				{"texture_rect", &GuiIonLoader::load_style_texture_rect},
			};
			const auto i = style_handlers.find(token.to_name());
			if (i == style_handlers.end())
				return false;
			(this->*i->second)(data, ion, token.next(ion));
			return true;
		};

		std::unique_ptr<WidgetData> data;
		if (token.type() == IonReader::Token::Type::ListBegin)
		{
			const auto i = _prototypes.find(strings::from_view(token.next(ion).to_value()));
			if (i == _prototypes.end())
				throw GuiDataError{"Unknown class \"", token.text(), "\""};
			data = std::make_unique<WidgetData>(*i->second);
			token.next(ion).check_list_end();
			token.next(ion);
		}
		else
		{
			data = std::make_unique<WidgetData>();
			auto& foreground = data->_styles[WidgetData::Style::Normal]._foreground;
			foreground.font = _default_font;
			foreground.font_texture = _default_font_texture;
		}
		token.check_object_begin();
		for (token.next(ion); token.type() != IonReader::Token::Type::ObjectEnd;)
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
				for (token.next(ion); token.type() != IonReader::Token::Type::ObjectEnd;)
					if (!load_style_entry(k->second))
						throw GuiDataError{"Unknown '", j->first, "' entry '", token.text(), "'"};
				token.next(ion);
			}
			else if (!load_style_entry(data->_styles[WidgetData::Style::Normal]))
				throw GuiDataError{"Unknown widget entry '", token.text(), "'"};
		}
		token.next(ion);
		return data;
	}

	void GuiIonLoader::load_widget_on_click(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		data._actions[WidgetData::Action::OnClick] = ::read_actions(ion, token);
	}

	void GuiIonLoader::load_widget_on_enter(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		data._actions[WidgetData::Action::OnEnter] = ::read_actions(ion, token);
	}

	void GuiIonLoader::load_widget_on_update(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		data._actions[WidgetData::Action::OnUpdate] = ::read_actions(ion, token);
	}

	void GuiIonLoader::load_widget_position(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		if (!::update_rect(data._rect, ion, token))
			throw GuiDataError{"Bad 'position'"};
	}

	void GuiIonLoader::load_widget_sound(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		data._sound = _gui.resource_loader().load_sound(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_widget_state(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		static const std::unordered_map<std::string_view, WidgetData::Style> states
		{
			{"checked", WidgetData::Style::Checked},
			{"disabled", WidgetData::Style::Disabled},
			{"hovered", WidgetData::Style::Hovered},
			{"normal", WidgetData::Style::Normal},
			{"pressed", WidgetData::Style::Pressed},
		};

		const auto i = states.find(token.to_value());
		if (i == states.end()) // TODO-17: Use init-statement.
			throw GuiDataError{"Unknown 'state' \"", token.text(), "\""};
		data._fixed_style = i->second;
		token.next(ion);
	}

	void GuiIonLoader::load_widget_text(WidgetData& data, IonReader& ion, IonReader::Token& token) const
	{
		data._text = token.translatable() ? _gui.translate(token.to_value()) : strings::from_view(token.to_value());
		token.next(ion);
	}

	void GuiIonLoader::load_style_align(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		static const std::unordered_map<std::string_view, unsigned> alignment_flags
		{
			{"bottom", BottomAlignment},
			{"center", CenterAlignment},
			{"left", LeftAlignment},
			{"right", RightAlignment},
			{"top", TopAlignment},
		};

		const auto i = alignment_flags.find(token.to_value());
		if (i == alignment_flags.end())
			throw GuiDataError{"Bad 'align' \"", token.text(), "\""};
		auto alignment = i->second;
		if (token.next(ion).type() == IonReader::Token::Type::Value)
		{
			if (alignment != TopAlignment && alignment != BottomAlignment)
				throw GuiDataError{"Bad 'align' \"", i->first, "\" \"", token.text(), "\""};
			const auto j = alignment_flags.find(token.text());
			if (j == alignment_flags.end())
				throw GuiDataError{"Bad 'align' \"", i->first, "\" \"", token.text(), "\""};
			if (j->second != LeftAlignment && j->second != RightAlignment)
				throw GuiDataError{"Bad 'align' \"", i->first, "\" \"", token.text(), "\""};
			alignment |= j->second;
			token.next(ion);
		}
		data._foreground.alignment = alignment;
	}

	void GuiIonLoader::load_style_borders(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		auto& borders = data._background.borders;
		if (!strings::to_number(token.to_value(), borders._top)
			|| !strings::to_number(token.next(ion).to_value(), borders._right)
			|| !strings::to_number(token.next(ion).to_value(), borders._bottom)
			|| !strings::to_number(token.next(ion).to_value(), borders._left))
			throw GuiDataError{"Bad 'borders'"};
		token.next(ion);
	}

	void GuiIonLoader::load_style_color(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		if (!::update_color(data._background.color, ion, token))
			throw GuiDataError{"Bad 'color'"};
	}

	void GuiIonLoader::load_style_font(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		const auto font_desc = _gui.font(strings::from_view(token.to_value()));
		if (!font_desc)
			throw GuiDataError{"Unknown font \"", token.text(), "\""};
		auto& foreground = data._foreground;
		foreground.font = font_desc->font;
		foreground.font_texture = font_desc->texture;
		token.next(ion);
	}

	void GuiIonLoader::load_style_text_color(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		if (!::update_color(data._foreground.color, ion, token))
			throw GuiDataError{"Bad 'text_color'"};
	}

	void GuiIonLoader::load_style_text_size(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		if (!strings::to_number(token.to_value(), data._foreground.size))
			throw GuiDataError{"Bad 'text_size'"};
		token.next(ion);
	}

	void GuiIonLoader::load_style_texture(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		auto& background = data._background;
		background.texture = _gui.resource_loader().load_texture_2d(token.to_value());
		if (!read_texture_filter(ion, token.next(ion), background.texture_filter))
			throw GuiDataError{"Bad 'texture' filter '", token.text(), "'"};
		background.texture_rect = RectF{Rect{background.texture->size()}};
	}

	void GuiIonLoader::load_style_texture_rect(WidgetData::StyleData& data, IonReader& ion, IonReader::Token& token) const
	{
		if (!::update_rect(data._background.texture_rect, ion, token))
			throw GuiDataError{"Bad 'texture_rect'"};
	}
}
