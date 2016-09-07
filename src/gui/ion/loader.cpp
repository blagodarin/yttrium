#include "loader.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>
#include <yttrium/ion/value.h>
#include <yttrium/log.h>
#include <yttrium/texture_cache.h>
#include "../exceptions.h"
#include "../gui.h"
#include "../layer.h"
#include "../widgets/widget.h"
#include "property_loader.h"

namespace Yttrium
{
	namespace
	{
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

		enum GuiAttribute : unsigned
		{
			IsDefault     = 1 << 0,
			IsRoot        = 1 << 1,
			IsTransparent = 1 << 2,
		};

		const std::pair<StaticString, GuiAttribute> _root_attributes[] =
		{
			{"default"_s, IsDefault},
			{"root"_s, IsRoot},
			{"transparent"_s, IsTransparent},
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

	GuiIonLoader::GuiIonLoader(GuiPrivate& gui)
		: _gui(gui)
		, _allocator(_gui.allocator())
		, _classes(_allocator)
		, _default_font_name(&_allocator)
	{
	}

	bool GuiIonLoader::load(const StaticString& source_name)
	{
		try
		{
			load_impl(source_name);
			return true;
		}
		catch (const GuiError& e)
		{
			const auto& source_name = e.source_name();
			if (source_name.is_empty())
				Log() << e.what();
			else
				Log() << "(" << e.source_name() << ") " << e.what();
		}
		_gui.clear();
		return false;
	}

	void GuiIonLoader::load_impl(const StaticString& source_name)
	{
		IonDocument document(_allocator);
		if (!document.load(source_name))
			throw GuiError(_allocator) << "Can't load \""_s << source_name << "\"..."_s;
		try
		{
			load(document.root());
		}
		catch (GuiError& e)
		{
			e.set_file(source_name);
			throw std::move(e);
		}
	}

	void GuiIonLoader::load(const IonObject& source)
	{
		static const std::map<StaticString, void (GuiIonLoader::*)(const IonNode&, unsigned)> handlers =
		{
			{"class"_s, &GuiIonLoader::load_class},
			{"font"_s, &GuiIonLoader::load_font},
			{"include"_s, &GuiIonLoader::load_include},
			{"layer"_s, &GuiIonLoader::load_layer},
		};

		unsigned attributes = 0;
		for (const auto& node : source)
		{
			if (node.is_empty())
			{
				const auto attribute = find_attribute(node.name());
				if (!attribute)
					throw GuiError(_allocator) << "Unknown attribute '"_s << node.name() << "'"_s;
				attributes |= attribute;
			}
			else
			{
				const auto i = handlers.find(node.name());
				if (i == handlers.end())
					throw GuiError(_allocator) << "Unknown entry '"_s << node.name() << "'"_s;
				(this->*i->second)(node, attributes);
				attributes = 0;
			}
		}
	}

	void GuiIonLoader::load_class(const IonNode& node, unsigned)
	{
		const auto& element = load_element(node);
		if (!element.object || !element.name)
			throw GuiError(_allocator) << "Bad 'class'"_s;
		if (!_classes.add(*element.name, *element.object, element.attribute))
			throw GuiError(_allocator) << "Bad 'class' \""_s << *element.name << "\""_s;
	}

	void GuiIonLoader::load_font(const IonNode& node, unsigned attributes)
	{
		auto element = load_element(node);
		if (!element.object)
			throw GuiError(_allocator) << "Bad 'font'"_s;

		const auto& default_name = "default"_s;
		if (!element.name)
			element.name = &default_name;

		if (attributes & IsDefault)
		{
			if (_has_default_font)
				throw GuiError(_allocator) << "Default 'font' redefinition"_s;
			_default_font_name = *element.name;
			_has_default_font = true;
		}

		const StaticString* font_name;
		if (!GuiIonPropertyLoader::load_text(&font_name, element.object->last("file"_s)))
			throw GuiError(_allocator) << "Bad 'font' 'file'"_s;

		const StaticString* texture_name;
		if (!GuiIonPropertyLoader::load_text(&texture_name, element.object->last("texture"_s)))
			throw GuiError(_allocator) << "Bad 'font' 'texture'"_s;

		_gui.set_font(*element.name, *font_name, *texture_name);
	}

	void GuiIonLoader::load_include(const IonNode& node, unsigned)
	{
		const StaticString* path;
		if (!Ion::get(node, path))
			throw GuiError(_allocator) << "Bad 'include'"_s;
		load_impl(*path);
	}

	void GuiIonLoader::load_layer(const IonNode& node, unsigned attributes)
	{
		static const std::map<StaticString, std::pair<bool (GuiIonLoader::*)(GuiLayer&, const IonNode&, int) const, int>> handlers =
		{
			{"center"_s, {&GuiIonLoader::load_layer_layout, static_cast<int>(GuiLayout::Placement::Center)}},
			{"on_key"_s, {&GuiIonLoader::load_layer_on_key, 0}},
			{"on_pop"_s, {&GuiIonLoader::load_layer_action, static_cast<int>(GuiLayer::Action::Pop)}},
			{"on_push"_s, {&GuiIonLoader::load_layer_action, static_cast<int>(GuiLayer::Action::Push)}},
			{"stretch"_s, {&GuiIonLoader::load_layer_layout, static_cast<int>(GuiLayout::Placement::Stretch)}},
		};

		const auto& element = load_element(node);
		if (!element.object)
			throw GuiError(_allocator) << "Empty 'layer'"_s;

		const StaticString layer_name = element.name ? *element.name : StaticString();

		auto& layer = _gui.add_layer(layer_name, attributes == IsTransparent, attributes & IsRoot);
		for (const auto& layer_node : *element.object)
		{
			const auto i = handlers.find(layer_node.name());
			if (i == handlers.end())
				throw GuiError(_allocator) << "Unknown '"_s << node.name() << "' entry '"_s << layer_node.name() << "'"_s;
			if (!(this->*i->second.first)(layer, layer_node, i->second.second))
				throw GuiError(_allocator) << "Bad '"_s << node.name() << "' '"_s << layer_node.name() << "'"_s;
		}
	}

	bool GuiIonLoader::load_layer_action(GuiLayer& layer, const IonNode& node, int extra) const
	{
		const StaticString* action;
		if (!GuiIonPropertyLoader::load_text(&action, node))
			return false;
		layer.set_action(static_cast<GuiLayer::Action>(extra), ScriptCode(*action, _allocator));
		return true;
	}

	bool GuiIonLoader::load_layer_layout(GuiLayer& layer, const IonNode& node, int extra) const
	{
		static const std::map<StaticString, bool (GuiIonLoader::*)(GuiLayout&, const IonNode&) const> handlers =
		{
			{"size"_s, &GuiIonLoader::load_layout_size},
		};

		if (node.size() != 1 || node.first()->type() != IonValue::Type::Object)
			return false;
		auto& layout = layer.add_layout(static_cast<GuiLayout::Placement>(extra));
		for (const auto& layout_node : *node.first()->object())
		{
			const auto i = handlers.find(layout_node.name());
			if (i != handlers.end())
			{
				if (!(this->*i->second)(layout, layout_node))
					throw GuiError(_allocator) << "Bad '"_s << node.name() << "' '"_s << layout_node.name() << "'"_s;
				continue;
			}

			const auto& element = load_element(layout_node);
			if (!element.object)
				throw GuiError(_allocator) << "Bad layout entry '"_s << layout_node.name() << "'"_s;

			GuiIonPropertyLoader loader(element.object, (element.attribute ? _classes.find(*element.attribute) : nullptr), _gui);
			if (_has_default_font)
				loader.set_default_font_name(&_default_font_name);
			auto& widget = layout.add_widget(layout_node.name(), loader);

			if (element.name)
				widget.set_name(*element.name);

			layer.register_widget(widget);
		}
		return true;
	}

	bool GuiIonLoader::load_layer_on_key(GuiLayer& layer, const IonNode& node, int) const
	{
		const auto values = node.values();
		if (values.size() != 2 || values->type() != IonValue::Type::String || values.last().type() != IonValue::Type::String)
			return false;
		layer.bind(values->string(), values.last().string());
		return true;
	}

	bool GuiIonLoader::load_layout_size(GuiLayout& layout, const IonNode& node) const
	{
		SizeF size;
		if (!GuiIonPropertyLoader::load_size(size, node))
			return false;
		layout.set_size(size);
		return true;
	}
}
