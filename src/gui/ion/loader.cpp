#include "loader.h"

#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>
#include <yttrium/ion/value.h>
#include <yttrium/resource_loader.h>
#include <yttrium/std/optional.h>
#include <yttrium/storage/storage.h>
#include "../gui.h"
#include "../screen.h"
#include "../widgets/widget.h"
#include "property_loader.h"

namespace
{
	using namespace Yttrium;

	struct GuiElement
	{
		const IonObject* _object = nullptr;
		std::string_view _name;
		std::string_view _attribute;
	};

	GuiElement load_element(const IonList& source)
	{
		GuiElement result;

		auto value = source.begin();
		if (value == source.end())
			return {};

		if (value->type() == IonValue::Type::String)
		{
			value->get(result._name);
			++value;
			if (value == source.end())
				return {};
		}

		if (value->type() == IonValue::Type::List)
		{
			const auto& list = value->list();
			if (list.size() != 1 || !list.first()->get(result._attribute))
				return {};
			++value;
			if (value == source.end())
				return {};
		}

		if (value->type() != IonValue::Type::Object)
			return {};

		result._object = value->object();

		++value;
		if (value != source.end())
			return {};

		return result;
	}

	std::optional<std::tuple<std::string_view, GuiActions, GuiActions>> load_on_key(const IonList& source)
	{
		const auto key = source.begin();
		if (key == source.end() || key->type() != IonValue::Type::String)
			return {};
		const auto on_press = key.next();
		if (on_press == source.end() || on_press->type() != IonValue::Type::Object)
			return {};
		const auto on_release = on_press.next();
		if (on_release != source.end() && (on_release->type() != IonValue::Type::Object || on_release.next() != source.end()))
			return {};
		return std::make_tuple(key->string(), GuiIonPropertyLoader::load_actions(*on_press->object()),
			on_release != source.end() ? GuiIonPropertyLoader::load_actions(*on_release->object()) : GuiActions());
	}
}

namespace Yttrium
{
	GuiIonLoader::GuiIonLoader(GuiPrivate& gui)
		: _gui{gui}
	{
	}

	void GuiIonLoader::load(std::string_view source_name)
	{
		try
		{
			load(_gui.resource_loader().load_ion(source_name)->root());
		}
		catch (GuiDataError& e)
		{
			throw GuiDataError{source_name, ":", e.what()};
		}
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

	void GuiIonLoader::load(const IonObject& source)
	{
		static const std::map<std::string_view, void (GuiIonLoader::*)(const IonNode&, Flags<Attribute>)> handlers =
		{
			{"class", &GuiIonLoader::load_class},
			{"cursor", &GuiIonLoader::load_cursor},
			{"font", &GuiIonLoader::load_font},
			{"include", &GuiIonLoader::load_include},
			{"on_key", &GuiIonLoader::load_on_key},
			{"screen", &GuiIonLoader::load_screen},
			{"translation", &GuiIonLoader::load_translation},
		};

		Flags<Attribute> attributes;
		for (const auto& node : source)
		{
			if (node.is_empty())
			{
				const auto attribute = load_attribute(node.name());
				if (attribute == Attribute::Unknown)
					throw GuiDataError{"Unknown attribute '", node.name(), "'"};
				attributes |= attribute;
			}
			else
			{
				const auto i = handlers.find(node.name());
				if (i == handlers.end())
					throw GuiDataError{"Unknown entry '", node.name(), "'"};
				(this->*i->second)(node, attributes);
				attributes = {};
			}
		}
	}

	void GuiIonLoader::load_class(const IonNode& node, Flags<Attribute>)
	{
		const auto& element = ::load_element(node);
		if (!element._object || !element._name.data())
			throw GuiDataError{"Bad '", node.name(), "'"};
		if (element._attribute.data())
		{
			const auto attribute = strings::from_view(element._attribute);
			if (_classes.add(strings::from_view(element._name), *element._object, &attribute))
				return;
		}
		else if (_classes.add(strings::from_view(element._name), *element._object))
			return;
		throw GuiDataError{"Bad '", node.name(), "' \"", element._name, "\""};
	}

	void GuiIonLoader::load_cursor(const IonNode& node, Flags<Attribute>)
	{
		const auto values = node.values();
		if (values.size() != 1 || values->type() != IonValue::Type::Object)
			throw GuiDataError{"Bad '", node.name(), "'"};
		const auto& object = *node.first()->object();
		if (object.size() != 1)
			throw GuiDataError{"Bad '", node.name(), "'"};
		const auto& object_node = *object.begin();
		if (object_node.name() == "none")
		{
			if (!object_node.is_empty())
				throw GuiDataError{"Bad '", node.name(), ".", object_node.name(), "'"};
			_gui.set_default_cursor(GuiCursor::None);
		}
		else if (object_node.name() == "custom")
		{
			if (!object_node.is_empty())
				throw GuiDataError{"Bad '", node.name(), ".", object_node.name(), "'"};
			_gui.set_default_cursor(GuiCursor::Custom);
		}
		else if (object_node.name() == "texture")
		{
			if (object_node.size() != 1 || object_node.first()->type() != IonValue::Type::String)
				throw GuiDataError{"Bad '", node.name(), ".", object_node.name(), "'"};
			_gui.set_default_cursor(GuiCursor::Texture, object_node.first()->string());
		}
		else if (object_node.name() != "default")
			throw GuiDataError{"Bad '", node.name(), "'"};
	}

	void GuiIonLoader::load_font(const IonNode& node, Flags<Attribute> attributes)
	{
		auto element = ::load_element(node);
		if (!element._object)
			throw GuiDataError{"Bad '", node.name(), "'"};

		if (!element._name.data())
			element._name = "default";

		if (attributes & Attribute::Default)
		{
			if (_has_default_font)
				throw GuiDataError{"Default '", node.name(), "' redefinition"};
			_default_font_name = strings::from_view(element._name);
			_has_default_font = true;
		}

		std::string_view font_name;
		if (!GuiIonPropertyLoader::load_text(font_name, element._object->last("file")))
			throw GuiDataError{"Bad '", node.name(), "' 'file'"};

		std::string_view texture_name;
		if (!GuiIonPropertyLoader::load_text(texture_name, element._object->last("texture")))
			throw GuiDataError{"Bad '", node.name(), "' 'texture'"};

		_gui.set_font(strings::from_view(element._name), font_name, texture_name);
	}

	void GuiIonLoader::load_include(const IonNode& node, Flags<Attribute>)
	{
		std::string_view path;
		if (!Ion::get(node, path))
			throw GuiDataError{"Bad '", node.name(), "'"};
		load(path);
	}

	void GuiIonLoader::load_screen(const IonNode& node, Flags<Attribute> attributes)
	{
		static const std::map<std::string_view, std::pair<void (GuiIonLoader::*)(GuiScreen&, const IonNode&, int) const, int>> handlers =
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

		const auto& element = ::load_element(node);
		if (!element._object)
			throw GuiDataError{"Empty '", node.name(), "'"};

		if (!element._name.data())
			throw GuiDataError{"'", node.name(), "' must have a name"};

		auto& screen = _gui.add_screen(strings::from_view(element._name), attributes & Attribute::Transparent && !(attributes & Attribute::Root), attributes & Attribute::Root);
		for (const auto& screen_node : *element._object)
		{
			const auto i = handlers.find(screen_node.name());
			if (i == handlers.end())
				throw GuiDataError{"Unknown '", node.name(), "' entry '", screen_node.name(), "'"};
			(this->*i->second.first)(screen, screen_node, i->second.second);
		}
	}

	void GuiIonLoader::load_on_key(const IonNode& node, Flags<Attribute>)
	{
		auto binding = ::load_on_key(node);
		if (!binding)
			throw GuiDataError{"Bad '", node.name(), "'"};
		_gui.set_on_key(std::get<0>(*binding), std::move(std::get<1>(*binding)), std::move(std::get<2>(*binding)));
	}

	void GuiIonLoader::load_translation(const IonNode& node, Flags<Attribute>)
	{
		std::string_view path;
		if (!Ion::get(node, path))
			throw GuiDataError{"Bad '", node.name(), "'"};
		_gui.set_translation(path);
	}

	void GuiIonLoader::load_screen_cursor(GuiScreen& screen, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 1 || values->type() != IonValue::Type::Object)
			throw GuiDataError{"Bad '", node.name(), "'"};
		const auto& object = *node.first()->object();
		if (object.size() != 1)
			throw GuiDataError{"Bad '", node.name(), "'"};
		const auto& object_node = *object.begin();
		if (object_node.name() == "none")
		{
			if (!object_node.is_empty())
				throw GuiDataError{"Bad '", node.name(), ".", object_node.name(), "'"};
			screen.set_cursor(GuiCursor::None);
		}
		else if (object_node.name() == "custom")
		{
			if (!object_node.is_empty())
				throw GuiDataError{"Bad '", node.name(), ".", object_node.name(), "'"};
			screen.set_cursor(GuiCursor::Custom);
		}
		else if (object_node.name() == "texture")
		{
			if (object_node.size() != 1 || object_node.first()->type() != IonValue::Type::String)
				throw GuiDataError{"Bad '", node.name(), ".", object_node.name(), "'"};
			screen.set_cursor(GuiCursor::Texture, object_node.first()->string());
		}
		else if (object_node.name() != "default")
			throw GuiDataError{"Bad '", node.name(), "'"};
	}

	void GuiIonLoader::load_screen_layout(GuiScreen& screen, const IonNode& node, int extra) const
	{
		static const std::map<std::string_view, void (GuiIonLoader::*)(GuiLayout&, const IonNode&) const> handlers =
		{
			{"size", &GuiIonLoader::load_layout_size},
		};

		const auto placement = static_cast<GuiLayout::Placement>(extra);
		auto& layout = screen.add_layout(placement);

		if (placement == GuiLayout::Placement::Left || placement == GuiLayout::Placement::Right)
		{
			int height = 0;
			if (node.size() != 2 || node.first()->type() != IonValue::Type::String || node.last()->type() != IonValue::Type::Object
				|| !strings::to_number(node.first()->string(), height))
				throw GuiDataError{"Bad '", node.name(), "'"};
			layout.set_size({0, static_cast<float>(height)});
		}
		else
		{
			if (node.size() != 1 || node.first()->type() != IonValue::Type::Object)
				throw GuiDataError{"Bad '", node.name(), "'"};
		}

		for (const auto& layout_node : *node.last()->object())
		{
			const auto i = handlers.find(layout_node.name());
			if (i != handlers.end())
			{
				(this->*i->second)(layout, layout_node);
				continue;
			}

			const auto& element = ::load_element(layout_node);
			if (!element._object)
				throw GuiDataError{"Bad layout entry '", layout_node.name(), "'"};

			GuiIonPropertyLoader loader(element._object, (element._attribute.data() ? _classes.find(strings::from_view(element._attribute)) : nullptr), _gui);
			if (_has_default_font)
				loader.set_default_font_name(&_default_font_name);
			screen.register_widget(layout.add_widget(layout_node.name(), element._name.data() ? element._name : "", loader));
		}
	}

	void GuiIonLoader::load_screen_music(GuiScreen& screen, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 1 || values->type() != IonValue::Type::String)
			throw GuiDataError{"Bad '", node.name(), "'"};
		const auto music_name = values->string();
		screen.set_music(music_name.empty() ? nullptr : _gui.resource_loader().load_music(music_name));
	}

	void GuiIonLoader::load_screen_on_enter(GuiScreen& screen, const IonNode& node, int) const
	{
		screen.set_on_enter(GuiIonPropertyLoader::load_actions(node));
	}

	void GuiIonLoader::load_screen_on_event(GuiScreen& screen, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 2 || values->type() != IonValue::Type::String || values.last().type() != IonValue::Type::Object)
			throw GuiDataError{"Bad '", node.name(), "'"};
		screen.set_on_event(strings::from_view(values->string()), GuiIonPropertyLoader::load_actions(*values.last().object()));
	}

	void GuiIonLoader::load_screen_on_key(GuiScreen& screen, const IonNode& node, int) const
	{
		auto binding = ::load_on_key(node);
		if (!binding)
			throw GuiDataError{"Bad '", node.name(), "'"};
		screen.set_on_key(std::get<0>(*binding), std::move(std::get<1>(*binding)), std::move(std::get<2>(*binding)));
	}

	void GuiIonLoader::load_screen_on_return(GuiScreen& screen, const IonNode& node, int) const
	{
		screen.set_on_return(GuiIonPropertyLoader::load_actions(node));
	}

	void GuiIonLoader::load_layout_size(GuiLayout& layout, const IonNode& node) const
	{
		SizeF size;
		GuiIonPropertyLoader::load_size(size, node);
		layout.set_size(size);
	}
}
