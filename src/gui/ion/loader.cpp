#include "loader.h"

#include <yttrium/exceptions.h>
#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>
#include <yttrium/ion/value.h>
#include <yttrium/log.h>
#include <yttrium/resources/resource_loader.h>
#include <yttrium/storage/reader.h>
#include <yttrium/storage/storage.h>
#include "../gui.h"
#include "../layer.h"
#include "../widgets/widget.h"
#include "property_loader.h"

#include <boost/optional/optional.hpp>

namespace
{
	using namespace Yttrium;

	struct GuiElement
	{
		const IonObject* object = nullptr;
		const StaticString* name = nullptr;
		const StaticString* attribute = nullptr;
	};

	GuiElement load_element(const IonList& source)
	{
		GuiElement result;

		auto value = source.begin();
		if (value == source.end())
			return {};

		if (value->type() == IonValue::Type::String)
		{
			value->get(&result.name);
			++value;
			if (value == source.end())
				return {};
		}

		if (value->type() == IonValue::Type::List)
		{
			const auto& list = value->list();
			if (list.size() != 1 || !list.first()->get(&result.attribute))
				return {};
			++value;
			if (value == source.end())
				return {};
		}

		if (value->type() != IonValue::Type::Object)
			return {};

		result.object = value->object();

		++value;
		if (value != source.end())
			return {};

		return result;
	}

	boost::optional<std::tuple<StaticString, GuiActions, GuiActions>> load_on_key(const IonList& source)
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

	enum GuiAttribute : unsigned
	{
		IsDefault     = 1 << 0,
		IsRoot        = 1 << 1,
		IsTransparent = 1 << 2,
	};

	const std::pair<StaticString, GuiAttribute> _root_attributes[] =
	{
		{ "default"_s, IsDefault },
		{ "root"_s, IsRoot },
		{ "transparent"_s, IsTransparent },
	};

	unsigned find_attribute(const StaticString& value)
	{
		for (const auto& attribute : _root_attributes)
		{
			if (attribute.first == value)
				return attribute.second;
		}
		return 0;
	}
}

namespace Yttrium
{
	GuiIonLoader::GuiIonLoader(GuiPrivate& gui)
		: _gui(gui)
		, _allocator(_gui.allocator())
		, _classes(_allocator)
		, _default_font_name(&_allocator)
	{
	}

	void GuiIonLoader::load(const StaticString& source_name)
	{
		try
		{
			load(_gui.resource_loader().load_ion(source_name)->root());
		}
		catch (GuiDataError& e)
		{
			e.set_source(source_name);
			throw;
		}
	}

	void GuiIonLoader::load(const IonObject& source)
	{
		static const std::map<StaticString, void (GuiIonLoader::*)(const IonNode&, unsigned)> handlers =
		{
			{ "class"_s, &GuiIonLoader::load_class },
			{ "cursor"_s, &GuiIonLoader::load_cursor },
			{ "font"_s, &GuiIonLoader::load_font },
			{ "include"_s, &GuiIonLoader::load_include },
			{ "layer"_s, &GuiIonLoader::load_layer },
			{ "on_key"_s, &GuiIonLoader::load_on_key },
			{ "translation"_s, &GuiIonLoader::load_translation },
		};

		unsigned attributes = 0;
		for (const auto& node : source)
		{
			if (node.is_empty())
			{
				const auto attribute = ::find_attribute(node.name());
				if (!attribute)
					throw GuiDataError("Unknown attribute '"_s, node.name(), "'"_s);
				attributes |= attribute;
			}
			else
			{
				const auto i = handlers.find(node.name());
				if (i == handlers.end())
					throw GuiDataError("Unknown entry '"_s, node.name(), "'"_s);
				(this->*i->second)(node, attributes);
				attributes = 0;
			}
		}
	}

	void GuiIonLoader::load_class(const IonNode& node, unsigned)
	{
		const auto& element = ::load_element(node);
		if (!element.object || !element.name)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		if (!_classes.add(*element.name, *element.object, element.attribute))
			throw GuiDataError("Bad '"_s, node.name(), "' \""_s, *element.name, "\""_s);
	}

	void GuiIonLoader::load_cursor(const IonNode& node, unsigned)
	{
		const auto values = node.values();
		if (values.size() != 1 || values->type() != IonValue::Type::String)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		if (values->string() == "none"_s)
			_gui.set_default_cursor(GuiCursor::None);
		else if (values->string() == "custom"_s)
			_gui.set_default_cursor(GuiCursor::Custom);
		else if (values->string() != "default"_s)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
	}

	void GuiIonLoader::load_font(const IonNode& node, unsigned attributes)
	{
		auto element = ::load_element(node);
		if (!element.object)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);

		const auto& default_name = "default"_s;
		if (!element.name)
			element.name = &default_name;

		if (attributes & IsDefault)
		{
			if (_has_default_font)
				throw GuiDataError("Default '"_s, node.name(), "' redefinition"_s);
			_default_font_name = *element.name;
			_has_default_font = true;
		}

		const StaticString* font_name;
		if (!GuiIonPropertyLoader::load_text(&font_name, element.object->last("file"_s)))
			throw GuiDataError("Bad '"_s, node.name(), "' 'file'"_s);

		const StaticString* texture_name;
		if (!GuiIonPropertyLoader::load_text(&texture_name, element.object->last("texture"_s)))
			throw GuiDataError("Bad '"_s, node.name(), "' 'texture'"_s);

		_gui.set_font(*element.name, *font_name, *texture_name);
	}

	void GuiIonLoader::load_include(const IonNode& node, unsigned)
	{
		const StaticString* path;
		if (!Ion::get(node, path))
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		load(*path);
	}

	void GuiIonLoader::load_layer(const IonNode& node, unsigned attributes)
	{
		static const std::map<StaticString, std::pair<void (GuiIonLoader::*)(GuiLayer&, const IonNode&, int) const, int>> handlers =
		{
			{ "center"_s, { &GuiIonLoader::load_layer_layout, static_cast<int>(GuiLayout::Placement::Center) } },
			{ "cursor"_s, { &GuiIonLoader::load_layer_cursor, 0 } },
			{ "music"_s, { &GuiIonLoader::load_layer_music, 0 } },
			{ "on_enter"_s, { &GuiIonLoader::load_layer_on_enter, 0 } },
			{ "on_event"_s, { &GuiIonLoader::load_layer_on_event, 0 } },
			{ "on_key"_s, { &GuiIonLoader::load_layer_on_key, 0 } },
			{ "on_return"_s, { &GuiIonLoader::load_layer_on_return, 0 } },
			{ "stretch"_s, { &GuiIonLoader::load_layer_layout, static_cast<int>(GuiLayout::Placement::Stretch) } },
		};

		const auto& element = ::load_element(node);
		if (!element.object)
			throw GuiDataError("Empty '"_s, node.name(), "'"_s);

		const StaticString layer_name = element.name ? *element.name : StaticString();

		auto& layer = _gui.add_layer(layer_name, attributes == IsTransparent, attributes & IsRoot);
		layer.set_cursor(_gui.default_cursor());
		for (const auto& layer_node : *element.object)
		{
			const auto i = handlers.find(layer_node.name());
			if (i == handlers.end())
				throw GuiDataError("Unknown '"_s, node.name(), "' entry '"_s, layer_node.name(), "'"_s);
			(this->*i->second.first)(layer, layer_node, i->second.second);
		}
	}

	void GuiIonLoader::load_on_key(const IonNode& node, unsigned)
	{
		auto binding = ::load_on_key(node);
		if (!binding)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		_gui.set_on_key(std::get<0>(*binding), std::move(std::get<1>(*binding)), std::move(std::get<2>(*binding)));
	}

	void GuiIonLoader::load_translation(const IonNode& node, unsigned)
	{
		const StaticString* path;
		if (!Ion::get(node, path))
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		_gui.set_translation(*path);
	}

	void GuiIonLoader::load_layer_cursor(GuiLayer& layer, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 1 || values->type() != IonValue::Type::String)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		if (values->string() == "none"_s)
			layer.set_cursor(GuiCursor::None);
		else if (values->string() == "custom"_s)
			layer.set_cursor(GuiCursor::Custom);
		else if (values->string() != "default"_s)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
	}

	void GuiIonLoader::load_layer_layout(GuiLayer& layer, const IonNode& node, int extra) const
	{
		static const std::map<StaticString, void (GuiIonLoader::*)(GuiLayout&, const IonNode&) const> handlers =
		{
			{ "size"_s, &GuiIonLoader::load_layout_size },
		};

		if (node.size() != 1 || node.first()->type() != IonValue::Type::Object)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		auto& layout = layer.add_layout(static_cast<GuiLayout::Placement>(extra));
		for (const auto& layout_node : *node.first()->object())
		{
			const auto i = handlers.find(layout_node.name());
			if (i != handlers.end())
			{
				(this->*i->second)(layout, layout_node);
				continue;
			}

			const auto& element = ::load_element(layout_node);
			if (!element.object)
				throw GuiDataError("Bad layout entry '"_s, layout_node.name(), "'"_s);
			if (element.name)
				throw GuiDataError("Widget names are not supported"_s);

			GuiIonPropertyLoader loader(element.object, (element.attribute ? _classes.find(*element.attribute) : nullptr), _gui);
			if (_has_default_font)
				loader.set_default_font_name(&_default_font_name);
			layer.register_widget(layout.add_widget(layout_node.name(), loader));
		}
	}

	void GuiIonLoader::load_layer_music(GuiLayer& layer, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 1 || values->type() != IonValue::Type::String)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		layer.set_music(_gui.resource_loader().load_music(values->string()));
	}

	void GuiIonLoader::load_layer_on_enter(GuiLayer& layer, const IonNode& node, int) const
	{
		layer.set_on_enter(GuiIonPropertyLoader::load_actions(node));
	}

	void GuiIonLoader::load_layer_on_event(GuiLayer& layer, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 2 || values->type() != IonValue::Type::String || values.last().type() != IonValue::Type::Object)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		layer.set_on_event(values->string().to_std(), GuiIonPropertyLoader::load_actions(*values.last().object()));
	}

	void GuiIonLoader::load_layer_on_key(GuiLayer& layer, const IonNode& node, int) const
	{
		auto binding = ::load_on_key(node);
		if (!binding)
			throw GuiDataError("Bad '"_s, node.name(), "'"_s);
		layer.set_on_key(std::get<0>(*binding), std::move(std::get<1>(*binding)), std::move(std::get<2>(*binding)));
	}

	void GuiIonLoader::load_layer_on_return(GuiLayer& layer, const IonNode& node, int) const
	{
		layer.set_on_return(GuiIonPropertyLoader::load_actions(node));
	}

	void GuiIonLoader::load_layout_size(GuiLayout& layout, const IonNode& node) const
	{
		SizeF size;
		GuiIonPropertyLoader::load_size(size, node);
		layout.set_size(size);
	}
}
