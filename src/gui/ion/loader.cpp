#include "loader.h"

#include <yttrium/ion/document.h>
#include <yttrium/ion/node.h>
#include <yttrium/ion/object.h>
#include <yttrium/ion/utils.h>
#include <yttrium/ion/value.h>
#include <yttrium/log.h>
#include <yttrium/texture_cache.h>
#include "../gui.h"
#include "../layer.h"
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

	GuiIonLoader::GuiIonLoader(GuiImpl& gui)
		: _gui(gui)
		, _classes(_gui.allocator())
		, _default_font_name(&_gui.allocator())
	{
	}

	bool GuiIonLoader::load(const StaticString& source_name, bool is_internal)
	{
		IonDocument document(&_gui.allocator());

		if (!document.load(source_name))
		{
			Log() << "(gui) Can't load \""_s << source_name << "\"..."_s;
			return false;
		}

		const auto& document_root = document.root();

		if (!is_internal)
		{
			SizeF size;
			if (GuiIonPropertyLoader::load_size(size, document_root.last("size"_s)))
				_gui.set_size(size);

			Scaling scaling;
			if (GuiIonPropertyLoader::load_scaling(&scaling, document_root.last("scale"_s)))
				_gui.set_scaling(scaling);
		}

		load(document_root);

		return true;
	}

	void GuiIonLoader::load(const IonObject& source)
	{
		unsigned attributes = 0;
		for (const auto& node : source)
		{
			if (node.is_empty())
			{
				const auto attribute = find_attribute(node.name());
				if (!attribute)
					Log() << "(gui) Unknown attribute \""_s << node.name() << "\""_s;
				else
					attributes |= attribute;
				continue;
			}
			if (node.name() == "include"_s)
			{
				const StaticString* include_path;
				if (Ion::get(node, include_path))
					load(*include_path, true);
			}
			else if (node.name() == "class"_s)
			{
				const auto& element = load_element(node);
				if (!element.object || !element.name)
					continue;
				if (!_classes.add(*element.name, *element.object, element.attribute))
					Log() << "(gui) Can' load class \""_s << *element.name << "\""_s;
			}
			else if (node.name() == "font"_s)
			{
				auto element = load_element(node);
				if (!element.object)
					continue;

				const auto& default_name = "default"_s;
				if (!element.name)
					element.name = &default_name;

				if (attributes & IsDefault)
				{
					if (_has_default_font)
					{
						Log() << "(gui) Default font redefinition ignored"_s;
						element.attribute = nullptr;
					}
					else
					{
						_has_default_font = true;
						_default_font_name = *element.name;
					}
				}

				const StaticString* font_name;
				const StaticString* texture_name;

				if (!(GuiIonPropertyLoader::load_text(&font_name, element.object->last("file"_s))
					&& GuiIonPropertyLoader::load_text(&texture_name, element.object->last("texture"_s))))
				{
					continue;
				}

				_gui.set_font(*element.name, *font_name, *texture_name);
			}
			else if (node.name() == "layer"_s)
			{
				const auto& element = load_element(node);
				StaticString layer_name = element.name ? *element.name : StaticString();

				if (!element.object)
				{
					Log() << "(gui) Ignored empty layer \""_s << layer_name << "\""_s;
					continue;
				}

				const bool is_root = attributes & IsRoot;
				if (!is_root && layer_name.is_empty())
				{
					Log() << "(gui) Ignored unnamed non-root layer"_s;
					continue;
				}

				auto layer = _gui.create_layer(layer_name, attributes == IsTransparent);
				if (layer)
				{
					load_layer(*layer, *element.object);
					_gui.add_layer(std::move(layer), is_root);
				}
			}
			attributes = 0;
		}
	}

	void GuiIonLoader::load_layer(GuiLayer& layer, const IonObject& source) const
	{
		layer.reserve(source.size());

		for (const auto& node : source)
		{
			if (node.name() == "size"_s)
			{
				SizeF size;
				if (GuiIonPropertyLoader::load_size(size, node))
					layer.set_size(size);
			}
			else if (node.name() == "scale"_s)
			{
				Scaling scaling;
				if (GuiIonPropertyLoader::load_scaling(&scaling, node))
					layer.set_scaling(scaling);
			}
			else if (node.name() == "bind"_s)
			{
				const auto s = node.values();
				if (s.size() == 2 && s->type() == IonValue::Type::String && s.last().type() == IonValue::Type::String)
					layer.bind(s->string(), s.last().string());
			}
			else if (node.name() == "on_push"_s)
			{
				const StaticString* action;
				if (GuiIonPropertyLoader::load_text(&action, node))
					layer.set_push_action(ScriptCode(*action, &_gui.allocator()));
			}
			else if (node.name() == "on_pop"_s)
			{
				const StaticString* action;
				if (GuiIonPropertyLoader::load_text(&action, node))
					layer.set_pop_action(ScriptCode(*action, &_gui.allocator()));
			}
			else
			{
				const auto& element = load_element(node);
				if (!element.object)
					continue;

				GuiIonPropertyLoader loader(element.object, (element.attribute ? _classes.find(*element.attribute) : nullptr), _gui);
				if (_has_default_font)
					loader.set_default_font_name(&_default_font_name);

				layer.load_widget(node.name(), (element.name ? *element.name : StaticString()), loader);
			}
		}
	}
}
